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
#endif