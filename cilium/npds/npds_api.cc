#include "cilium/npds/npds_api.h"

#include <string>
#include <vector>

#include "envoy/stats/scope.h"

#include "source/common/common/assert.h"
#include "source/common/protobuf/utility.h"

#include "cilium/api/npds.pb.h"
#include "cilium/grpc_subscription.h"

namespace Envoy {
namespace Cilium {

constexpr absl::string_view NPDSTypeURL  = "type.googleapis.com/cilium.NetworkPolicy"

NpdsApi::NpdsApi(const envoy::config::core::v3::ConfigSource& npds_config, Server::Configuration::ServerFactoryContext& server_context,
            Init::Manager& init_manager, Stats::Scope& scope)
    : Envoy::Config::SubscriptionBase<cilium::NetworkPolicy>(
          server_context.messageValidationContext().dynamicValidationVisitor(), "endpoint_id"),
      scope_(scope.createScope("cilium.npds.")),
      init_target_("NPDS", [this]() { subscription_->start({}); }) {
  const auto resource_name = getResourceName();
  subscription_ = THROW_OR_RETURN_VALUE(factory_context.clusterManager().subscriptionFactory().subscriptionFromConfigSource(
                                            npds_config, Grpc::Common::typeUrl(resource_name),
                                            *scope_, *this, resource_decoder_, {}),
                                        Config::SubscriptionPtr);
  subscription_ =
      subscribe(NPDSTypeURL, server_context.localInfo(),
                server_context.clusterManager(), server_context.mainThreadDispatcher(),
                server_context.api().randomGenerator(), *scope_, *this, resource_decoder_);
  init_manager.add(init_target_);
}

absl::Status
NpdsApi::onConfigUpdate(const std::vector<Config::DecodedResourceRef>& added_resources,
                            const Protobuf::RepeatedPtrField<std::string>& removed_resources,
                            const std::string& system_version_info) {
  // Delegate incremental updates to the network policy map
  auto status =
      network_policy_map_.onConfigUpdate(added_resources, removed_resources, system_version_info);
  if (status.ok()) {
    system_version_info_ = system_version_info;
  }
  init_target_.ready();
  return status;
}

absl::Status
NpdsApi::onConfigUpdate(const std::vector<Config::DecodedResourceRef>& resources,
                            const std::string& version_info) {
  // Delegate SotW updates to the network policy map
  auto status = network_policy_map_.onConfigUpdate(resources, version_info);
  if (status.ok()) {
    system_version_info_ = version_info;
  }
  init_target_.ready();
  return status;
}

void NpdsApi::onConfigUpdateFailed(Envoy::Config::ConfigUpdateFailureReason reason,
                                       const EnvoyException*) {
  ASSERT(Envoy::Config::ConfigUpdateFailureReason::ConnectionFailure != reason);
  // Allow server startup to continue even with a bad config.
  init_target_.ready();
}

} // namespace Cilium
} // namespace Envoy
