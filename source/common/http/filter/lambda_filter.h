#pragma once

#include <map>
#include <string>

#include "envoy/upstream/cluster_manager.h"

#include "common/http/filter/aws_authenticator.h"
#include "common/http/filter/function.h"
#include "common/http/filter/function_retriever.h"
#include "common/http/filter/lambda_filter_config.h"
#include "common/http/functional_stream_decoder_base.h"

#include "server/config/network/http_connection_manager.h"

#include "lambda_filter.pb.h"

namespace Envoy {
namespace Http {

class LambdaFilter : public FunctionalFilterBase {
public:
  LambdaFilter(Server::Configuration::FactoryContext &ctx,
               const std::string &name, LambdaFilterConfigSharedPtr config,
               FunctionRetrieverSharedPtr retreiver);
  ~LambdaFilter();

  // Http::FunctionalFilterBase
  FilterHeadersStatus functionDecodeHeaders(HeaderMap &, bool) override;
  FilterDataStatus functionDecodeData(Buffer::Instance &, bool) override;
  FilterTrailersStatus functionDecodeTrailers(HeaderMap &) override;
  bool retrieveFunction(const MetadataAccessor &meta_accessor) override;

private:
  static const LowerCaseString INVOCATION_TYPE;
  static const std::string INVOCATION_TYPE_EVENT;
  static const std::string INVOCATION_TYPE_REQ_RESP;

  static const LowerCaseString LOG_TYPE;
  static const std::string LOG_NONE;

  const LambdaFilterConfigSharedPtr config_;
  FunctionRetrieverSharedPtr function_retriever_;

  Optional<Function> current_function_;
  void lambdafy();
  std::string functionUrlPath();
  void cleanup();

  Envoy::Http::HeaderMap *request_headers_{};
  AwsAuthenticator aws_authenticator_;
};

} // namespace Http
} // namespace Envoy
