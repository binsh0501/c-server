#ifndef __STATUS_H__
#define __STATUS_H__
typedef int HTTPStatus;
  #define HttpContinue 100
  #define HttpSwitchingProtocol 101
  #define HttpProcessing 102
  #define HttpEarlyHints 103
  #define HttpOk 200
  #define HttpCreated 201
  #define HttpAccepted 202
  #define HttpNonAuthoritativeInformation 203
  #define HttpNoContent 204
  #define HttpResetContent 205
  #define HttpPartialContent 206
  #define HttpMultiStatus 207
  #define HttpIMUsed 226
  #define HttpMultipleChoice 300
  #define HttpMovedPermanently 301
  #define HttpFound 302
  #define HttpSeeOther 303
  #define HttpNotModified 304
  #define HttpUseProxy 305  /* Don't use this */
  #define HttpTemporaryRedirect 307
  #define HttpPermanentRedirect 308
  #define HttpBadRequest 400
  #define HttpUnauthorized 401
  #define HttpPaymentRequired 402
  #define HttpForbidden 403
  #define HttpNotFound 404
  #define HttpMethodNotAllowed 405
  #define HttpNotAcceptable 406
  #define HttpProxyAuthenticationRequired 407
  #define HttpRequestTimeout 408
  #define HttpConflict 409
  #define HttpGone 410
  #define HttpLengthRequired 411
  #define HttpPreconditionFailed 412
  #define HttpPayloadTooLarge 413
  #define HttpUriTooLong 414
  #define HttpUnsupportedMediaType 415
  #define HttpRangeNotSatisfiable 416
  #define HttpExpectationFailed 417
  #define HttpMisdirectedRequest 421
  #define HttpUnprocessableEntity 422
  #define HttpLocked 423
  #define HttpFailedDependency 424
  #define HttpUpgradeRequired 426
  #define HttpPreconditionRequired 428
  #define HttpTooManyRequests 429
  #define HttpRequestHeaderFieldsTooLarge 431
  #define HttpUnavailableForLegalReasons 451
  #define HttpInternalServerError 500
  #define HttpNotImplemented 501
  #define HttpBadGateway 502
  #define HttpServiceUnavailable 503
  #define HttpGatewayTimeout 504
  #define HttpVersionNotSupported 505
  #define HttpVariantAlsoNegotiates 506
  #define HttpInsufficientStorage 507
  #define HttpLoopDetected 508
  #define HttpNotExtended 510
  #define HttpNetworkAuthenticationRequired 511
typedef char* HTTPphrase;
  #define HttpContinuePhrase "Continue"
  #define HttpSwitchingProtocolPhrase "Switching Protocol"
  #define HttpProcessingPhrase "Processing"
  #define HttpEarlyHintsPhrase "Early Hints"
  #define HttpOkPhrase "OK"
  #define HttpCreatedPhrase "Created"
  #define HttpAcceptedPhrase "Accepted"
  #define HttpNonAuthoritativeInformationPhrase "Non-Authoritative Information"
  #define HttpNoContentPhrase "No Content"
  #define HttpResetContentPhrase "Reset Content"
  #define HttpPartialContentPhrase "Partial Content"
  #define HttpMultiStatusPhrase "Multi-Status"
  #define HttpIMUsedPhrase "IM Used"
  #define HttpMultipleChoicePhrase "Multiple Choice"
  #define HttpMovedPermanentlyPhrase "Moved Permanently"
  #define HttpFoundPhrase "Found"
  #define HttpSeeOtherPhrase "See Other"
  #define HttpNotModifiedPhrase "Not Modified"
  #define HttpUseProxyPhrase "Use Proxy"
  #define HttpTemporaryRedirectPhrase "Temporary Redirect"
  #define HttpPermanentRedirectPhrase "Permanent Redirect"
  #define HttpBadRequestPhrase "Bad Request"
  #define HttpUnauthorizedPhrase "Unauthorized"
  #define HttpPaymentRequiredPhrase "Payment Required"
  #define HttpForbiddenPhrase "Forbidden"
  #define HttpNotFoundPhrase "Not Found"
  #define HttpMethodNotAllowedPhrase "Method Not Allowed"
  #define HttpNotAcceptablePhrase "Not Acceptable"
  #define HttpProxyAuthenticationRequiredPhrase "Proxy Authentication Required"
  #define HttpRequestTimeoutPhrase "Request Timeout"
  #define HttpConflictPhrase "Conflict"
  #define HttpGonePhrase "Gone"
  #define HttpLengthRequiredPhrase "Length Required"
  #define HttpPreconditionFailedPhrase "Precondition Failed"
  #define HttpPayloadTooLargePhrase "Payload Too Large"
  #define HttpUriTooLongPhrase "URI Too Long"
  #define HttpUnsupportedMediaTypePhrase "Unsupported Media Type"
  #define HttpRangeNotSatisfiablePhrase "Range Not Satisfiable"
  #define HttpExpectationFailedPhrase "Expectation Failed"
  #define HttpMisdirectedRequestPhrase "Misdirected Request"
  #define HttpUnprocessableEntityPhrase "Unprocessable Entity"
  #define HttpLockedPhrase "Locked"
  #define HttpFailedDependencyPhrase "Failed Dependency"
  #define HttpUpgradeRequiredPhrase "Upgrade Required"
  #define HttpPreconditionRequiredPhrase "Precondition Required"
  #define HttpTooManyRequestsPhrase "Too Many Requests"
  #define HttpRequestHeaderFieldsTooLargePhrase "Request Header Fields Too Large"
  #define HttpUnavailableForLegalReasonsPhrase "Unavailable For Legal Reasons"
  #define HttpInternalServerErrorPhrase "Internal Server Error"
  #define HttpNotImplementedPhrase "Not Implemented"
  #define HttpBadGatewayPhrase "Bad Gateway"
  #define HttpServiceUnavailablePhrase "Service Unavailable"
  #define HttpGatewayTimeoutPhrase "Gateway Timeout"
  #define HttpVersionNotSupportedPhrase "HTTP Version Not Supported"
  #define HttpVariantAlsoNegotiatesPhrase "Variant Also Negotiates"
  #define HttpInsufficientStoragePhrase "Insufficient Storage"
  #define HttpLoopDetectedPhrase "Loop Detected"
  #define HttpNotExtendedPhrase "Not Extended"
  #define HttpNetworkAuthenticationRequiredPhrase "Network Authentication Required"

static struct { HTTPStatus status; HTTPphrase phrase; } HttpStatusTable[] = {
  {HttpContinue, HttpContinuePhrase},
  {HttpSwitchingProtocol, HttpSwitchingProtocolPhrase},
  {HttpProcessing, HttpProcessingPhrase},
  {HttpEarlyHints, HttpEarlyHintsPhrase},
  {HttpOk, HttpOkPhrase},
  {HttpCreated, HttpCreatedPhrase},
  {HttpAccepted, HttpAcceptedPhrase},
  {HttpNonAuthoritativeInformation, HttpNonAuthoritativeInformationPhrase},
  {HttpNoContent, HttpNoContentPhrase},
  {HttpResetContent, HttpResetContentPhrase},
  {HttpPartialContent, HttpPartialContentPhrase},
  {HttpMultiStatus, HttpMultiStatusPhrase},
  {HttpIMUsed, HttpIMUsedPhrase},
  {HttpMultipleChoice, HttpMultipleChoicePhrase},
  {HttpMovedPermanently, HttpMovedPermanentlyPhrase},
  {HttpFound, HttpFoundPhrase},
  {HttpSeeOther, HttpSeeOtherPhrase},
  {HttpNotModified, HttpNotModifiedPhrase},
  {HttpUseProxy, HttpUseProxyPhrase},
  {HttpTemporaryRedirect, HttpTemporaryRedirectPhrase},
  {HttpPermanentRedirect, HttpPermanentRedirectPhrase},
  {HttpBadRequest, HttpBadRequestPhrase},
  {HttpUnauthorized, HttpUnauthorizedPhrase},
  {HttpPaymentRequired, HttpPaymentRequiredPhrase},
  {HttpForbidden, HttpForbiddenPhrase},
  {HttpNotFound, HttpNotFoundPhrase},
  {HttpMethodNotAllowed, HttpMethodNotAllowedPhrase},
  {HttpNotAcceptable, HttpNotAcceptablePhrase},
  {HttpProxyAuthenticationRequired, HttpProxyAuthenticationRequiredPhrase},
  {HttpRequestTimeout, HttpRequestTimeoutPhrase},
  {HttpConflict, HttpConflictPhrase},
  {HttpGone, HttpGonePhrase},
  {HttpLengthRequired, HttpLengthRequiredPhrase},
  {HttpPreconditionFailed, HttpPreconditionFailedPhrase},
  {HttpPayloadTooLarge, HttpPayloadTooLargePhrase},
  {HttpUriTooLong, HttpUriTooLongPhrase},
  {HttpUnsupportedMediaType, HttpUnsupportedMediaTypePhrase},
  {HttpRangeNotSatisfiable, HttpRangeNotSatisfiablePhrase},
  {HttpExpectationFailed, HttpExpectationFailedPhrase},
  {HttpMisdirectedRequest, HttpMisdirectedRequestPhrase},
  {HttpUnprocessableEntity, HttpUnprocessableEntityPhrase},
  {HttpLocked, HttpLockedPhrase},
  {HttpFailedDependency, HttpFailedDependencyPhrase},
  {HttpUpgradeRequired, HttpUpgradeRequiredPhrase},
  {HttpPreconditionRequired, HttpPreconditionRequiredPhrase},
  {HttpTooManyRequests, HttpTooManyRequestsPhrase},
  {HttpRequestHeaderFieldsTooLarge, HttpRequestHeaderFieldsTooLargePhrase},
  {HttpUnavailableForLegalReasons, HttpUnavailableForLegalReasonsPhrase},
  {HttpInternalServerError, HttpInternalServerErrorPhrase},
  {HttpNotImplemented, HttpNotImplementedPhrase},
  {HttpBadGateway, HttpBadGatewayPhrase},
  {HttpServiceUnavailable, HttpServiceUnavailablePhrase},
  {HttpGatewayTimeout, HttpGatewayTimeoutPhrase},
  {HttpVersionNotSupported, HttpVersionNotSupportedPhrase},
  {HttpVariantAlsoNegotiates, HttpVariantAlsoNegotiatesPhrase},
  {HttpInsufficientStorage, HttpInsufficientStoragePhrase},
  {HttpLoopDetected, HttpLoopDetectedPhrase},
  {HttpNotExtended, HttpNotExtendedPhrase},
  {HttpNetworkAuthenticationRequired, HttpNetworkAuthenticationRequiredPhrase},
  {-1, NULL}
};
#endif
