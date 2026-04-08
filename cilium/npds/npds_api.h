#pragma once

#include <memory>
#include <string>

#include "envoy/config/subscription.h"
#include "envoy/init/manager.h"
#include "envoy/stats/scope.h"

#include "source/common/common/logger.h"
#include "source/common/config/subscription_base.h"
#include "source/common/init/target_impl.h"

#include "cilium/api/npds.pb.h"
#include "cilium/api/npds.pb.validate.h"
#include "cilium/network_policy.h"

namespace Envoy {
namespace Cilium {

/**
 * NPDS API implementation that fetches network policies via Subscription.
 */
class NpdsApi : public Envoy::Config::SubscriptionBase<cilium::NetworkPolicy>,
                    public Logger::Loggable<Logger::Id::config> {
public:
  NpdsApi(const envoy::config::core::v3::ConfigSource& npds_config, Server::Configuration::ServerFactoryContext& server_context,
              Init::Manager& init_manager, Stats::Scope& scope,
              NetworkPolicyMapImpl& network_policy_map);

  std::string versionInfo() const { return system_version_info_; }

private:
  // Config::SubscriptionCallbacks
  absl::Status onConfigUpdate(const std::vector<Config::DecodedResourceRef>& resources,
                               const std::string& version_info) override;
  absl::Status onConfigUpdate(const std::vector<Config::DecodedResourceRef>& added_resources,
                               const Protobuf::RepeatedPtrField<std::string>& removed_resources,
                               const std::string& system_version_info) override;
  void onConfigUpdateFailed(Envoy::Config::ConfigUpdateFailureReason reason,
                            const EnvoyException* e) override;

  Config::SubscriptionPtr subscription_;
  std::string system_version_info_;
  NetworkPolicyMapImpl& network_policy_map_;
  Stats::ScopeSharedPtr scope_;
  Init::TargetImpl init_target_;
};

using NpdsApiPtr = std::unique_ptr<NpdsApi>;

} // namespace Cilium
} // namespace Envoy
