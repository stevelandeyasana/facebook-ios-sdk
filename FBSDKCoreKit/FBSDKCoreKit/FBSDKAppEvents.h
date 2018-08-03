// Copyright (c) 2014-present, Facebook, Inc. All rights reserved.
//
// You are hereby granted a non-exclusive, worldwide, royalty-free license to use,
// copy, modify, and distribute this software in source code or binary form for use
// in connection with the web services and APIs provided by Facebook.
//
// As with any software that integrates with the Facebook platform, your use of
// this software is subject to the Facebook Developer Principles and Policies
// [http://developers.facebook.com/policy/]. This copyright notice shall be
// included in all copies or substantial portions of the software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
// FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
// COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
// IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
// CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

#import <Foundation/Foundation.h>

#if !TARGET_OS_TV
#import <WebKit/WebKit.h>
#endif

#import <FBSDKCoreKit/FBSDKGraphRequestConnection.h>

#import "FBSDKMacros.h"

@class FBSDKAccessToken;
@class FBSDKGraphRequest;

/**  NSNotificationCenter name indicating a result of a failed log flush attempt. The posted object will be an NSError instance. */
FBSDK_EXTERN NSString *const FBSDKAppEventsLoggingResultNotification;

/**  optional plist key ("FacebookLoggingOverrideAppID") for setting `loggingOverrideAppID` */
FBSDK_EXTERN NSString *const FBSDKAppEventsOverrideAppIDBundleKey;

/**

 NS_ENUM (NSUInteger, FBSDKAppEventsFlushBehavior)

  Specifies when `FBSDKAppEvents` sends log events to the server.

 */
typedef NS_ENUM(NSUInteger, FBSDKAppEventsFlushBehavior)
{

  /** Flush automatically: periodically (once a minute or every 100 logged events) and always at app reactivation. */
  FBSDKAppEventsFlushBehaviorAuto = 0,

  /** Only flush when the `flush` method is called. When an app is moved to background/terminated, the
   events are persisted and re-established at activation, but they will only be written with an
   explicit call to `flush`. */
  FBSDKAppEventsFlushBehaviorExplicitOnly,

};

/*
 @methodgroup Predefined values to assign to event parameters that accompany events logged through the `logEvent` family
 of methods on `FBSDKAppEvents`.  Common event parameters are provided in the `FBSDKAppEventParameterName*` constants.
 */

/** Yes-valued parameter value to be used with parameter keys that need a Yes/No value */
FBSDK_EXTERN NSString *const FBSDKAppEventParameterValueYes;

/** No-valued parameter value to be used with parameter keys that need a Yes/No value */
FBSDK_EXTERN NSString *const FBSDKAppEventParameterValueNo;


/**


  Client-side event logging for specialized application analytics available through Facebook App Insights
 and for use with Facebook Ads conversion tracking and optimization.



 The `FBSDKAppEvents` static class has a few related roles:

 + Logging predefined and application-defined events to Facebook App Insights with a
 numeric value to sum across a large number of events, and an optional set of key/value
 parameters that define "segments" for this event (e.g., 'purchaserStatus' : 'frequent', or
 'gamerLevel' : 'intermediate')

 + Logging events to later be used for ads optimization around lifetime value.

 + Methods that control the way in which events are flushed out to the Facebook servers.

 Here are some important characteristics of the logging mechanism provided by `FBSDKAppEvents`:

 + Events are not sent immediately when logged.  They're cached and flushed out to the Facebook servers
 in a number of situations:
 - when an event count threshold is passed (currently 100 logged events).
 - when a time threshold is passed (currently 15 seconds).
 - when an app has gone to background and is then brought back to the foreground.

 + Events will be accumulated when the app is in a disconnected state, and sent when the connection is
 restored and one of the above 'flush' conditions are met.

 + The `FBSDKAppEvents` class is thread-safe in that events may be logged from any of the app's threads.

 + The developer can set the `flushBehavior` on `FBSDKAppEvents` to force the flushing of events to only
 occur on an explicit call to the `flush` method.

 + The developer can turn on console debug output for event logging and flushing to the server by using
 the `FBSDKLoggingBehaviorAppEvents` value in `[FBSettings setLoggingBehavior:]`.

 Some things to note when logging events:

 + There is a limit on the number of unique event names an app can use, on the order of 1000.
 + There is a limit to the number of unique parameter names in the provided parameters that can
 be used per event, on the order of 25.  This is not just for an individual call, but for all
 invocations for that eventName.
 + Event names and parameter names (the keys in the NSDictionary) must be between 2 and 40 characters, and
 must consist of alphanumeric characters, _, -, or spaces.
 + The length of each parameter value can be no more than on the order of 100 characters.

 */
@interface FBSDKAppEvents : NSObject

/*
 * Basic event logging
 */

/**

  Log an event with just an eventName.

 - Parameter eventName:   The name of the event to record.  Limitations on number of events and name length
 are given in the `FBSDKAppEvents` documentation.

 */
+ (void)logEvent:(NSString *)eventName;

/**

  Log an event with an eventName and a numeric value to be aggregated with other events of this name.

 - Parameter eventName:   The name of the event to record.  Limitations on number of events and name length
 are given in the `FBSDKAppEvents` documentation.  Common event names are provided in `FBAppEventName*` constants.

 - Parameter valueToSum:  Amount to be aggregated into all events of this eventName, and App Insights will report
 the cumulative and average value of this amount.
 */
+ (void)logEvent:(NSString *)eventName
      valueToSum:(double)valueToSum;


/**

  Log an event with an eventName and a set of key/value pairs in the parameters dictionary.
 Parameter limitations are described above.

 - Parameter eventName:   The name of the event to record.  Limitations on number of events and name construction
 are given in the `FBSDKAppEvents` documentation.  Common event names are provided in `FBAppEventName*` constants.

 - Parameter parameters:  Arbitrary parameter dictionary of characteristics. The keys to this dictionary must
 be NSString's, and the values are expected to be NSString or NSNumber.  Limitations on the number of
 parameters and name construction are given in the `FBSDKAppEvents` documentation.  Commonly used parameter names
 are provided in `FBSDKAppEventParameterName*` constants.
 */
+ (void)logEvent:(NSString *)eventName
      parameters:(NSDictionary *)parameters;

/**

  Log an event with an eventName, a numeric value to be aggregated with other events of this name,
 and a set of key/value pairs in the parameters dictionary.

 - Parameter eventName:   The name of the event to record.  Limitations on number of events and name construction
 are given in the `FBSDKAppEvents` documentation.  Common event names are provided in `FBAppEventName*` constants.

 - Parameter valueToSum:  Amount to be aggregated into all events of this eventName, and App Insights will report
 the cumulative and average value of this amount.

 - Parameter parameters:  Arbitrary parameter dictionary of characteristics. The keys to this dictionary must
 be NSString's, and the values are expected to be NSString or NSNumber.  Limitations on the number of
 parameters and name construction are given in the `FBSDKAppEvents` documentation.  Commonly used parameter names
 are provided in `FBSDKAppEventParameterName*` constants.

 */
+ (void)logEvent:(NSString *)eventName
      valueToSum:(double)valueToSum
      parameters:(NSDictionary *)parameters;


/**

  Log an event with an eventName, a numeric value to be aggregated with other events of this name,
 and a set of key/value pairs in the parameters dictionary.  Providing session lets the developer
 target a particular <FBSession>.  If nil is provided, then `[FBSession activeSession]` will be used.

 - Parameter eventName:   The name of the event to record.  Limitations on number of events and name construction
 are given in the `FBSDKAppEvents` documentation.  Common event names are provided in `FBAppEventName*` constants.

 - Parameter valueToSum:  Amount to be aggregated into all events of this eventName, and App Insights will report
 the cumulative and average value of this amount.  Note that this is an NSNumber, and a value of `nil` denotes
 that this event doesn't have a value associated with it for summation.

 - Parameter parameters:  Arbitrary parameter dictionary of characteristics. The keys to this dictionary must
 be NSString's, and the values are expected to be NSString or NSNumber.  Limitations on the number of
 parameters and name construction are given in the `FBSDKAppEvents` documentation.  Commonly used parameter names
 are provided in `FBSDKAppEventParameterName*` constants.

 - Parameter accessToken:  The optional access token to log the event as.
 */
+ (void)logEvent:(NSString *)eventName
      valueToSum:(NSNumber *)valueToSum
      parameters:(NSDictionary *)parameters
     accessToken:(FBSDKAccessToken *)accessToken;

/**

  Notifies the events system that the app has launched and, when appropriate, logs an "activated app" event.
 This function is called automatically from FBSDKApplicationDelegate applicationDidBecomeActive, unless
 one overrides 'FacebookAutoLogAppEventsEnabled' key to false in the project info plist file.
 In case 'FacebookAutoLogAppEventsEnabled' is set to false, then it should typically be placed in the
 app delegates' `applicationDidBecomeActive:` method.

 This method also takes care of logging the event indicating the first time this app has been launched, which, among other things, is used to
 track user acquisition and app install ads conversions.



 `activateApp` will not log an event on every app launch, since launches happen every time the app is backgrounded and then foregrounded.
 "activated app" events will be logged when the app has not been active for more than 60 seconds.  This method also causes a "deactivated app"
 event to be logged when sessions are "completed", and these events are logged with the session length, with an indication of how much
 time has elapsed between sessions, and with the number of background/foreground interruptions that session had.  This data
 is all visible in your app's App Events Insights.
 */
+ (void)activateApp;

/*
 * Control over event batching/flushing
 */

/**

  Get the current event flushing behavior specifying when events are sent back to Facebook servers.
 */
+ (FBSDKAppEventsFlushBehavior)flushBehavior;

/**

  Set the current event flushing behavior specifying when events are sent back to Facebook servers.

 - Parameter flushBehavior:   The desired `FBSDKAppEventsFlushBehavior` to be used.
 */
+ (void)setFlushBehavior:(FBSDKAppEventsFlushBehavior)flushBehavior;

/**
  Set the 'override' App ID for App Event logging.



 In some cases, apps want to use one Facebook App ID for login and social presence and another
 for App Event logging.  (An example is if multiple apps from the same company share an app ID for login, but
 want distinct logging.)  By default, this value is `nil`, and defers to the `FBSDKAppEventsOverrideAppIDBundleKey`
 plist value.  If that's not set, it defaults to `[FBSDKSettings appID]`.

 This should be set before any other calls are made to `FBSDKAppEvents`.  Thus, you should set it in your application
 delegate's `application:didFinishLaunchingWithOptions:` delegate.

 - Parameter appID: The Facebook App ID to be used for App Event logging.
 */
+ (void)setLoggingOverrideAppID:(NSString *)appID;

/**
  Get the 'override' App ID for App Event logging.


- See:setLoggingOverrideAppID:

 */
+ (NSString *)loggingOverrideAppID;


/**
  Explicitly kick off flushing of events to Facebook.  This is an asynchronous method, but it does initiate an immediate
 kick off.  Server failures will be reported through the NotificationCenter with notification ID `FBSDKAppEventsLoggingResultNotification`.
 */
+ (void)flush;

/**
  Creates a request representing the Graph API call to retrieve a Custom Audience "third party ID" for the app's Facebook user.
 Callers will send this ID back to their own servers, collect up a set to create a Facebook Custom Audience with,
 and then use the resultant Custom Audience to target ads.

 - Parameter accessToken: The access token to use to establish the user's identity for users logged into Facebook through this app.
 If `nil`, then the `[FBSDKAccessToken currentAccessToken]` is used.



 The JSON in the request's response will include an "custom_audience_third_party_id" key/value pair, with the value being the ID retrieved.
 This ID is an encrypted encoding of the Facebook user's ID and the invoking Facebook app ID.
 Multiple calls with the same user will return different IDs, thus these IDs cannot be used to correlate behavior
 across devices or applications, and are only meaningful when sent back to Facebook for creating Custom Audiences.

 The ID retrieved represents the Facebook user identified in the following way: if the specified access token is valid,
 the ID will represent the user associated with that token; otherwise the ID will represent the user logged into the
 native Facebook app on the device.  If there is no native Facebook app, no one is logged into it, or the user has opted out
 at the iOS level from ad tracking, then a `nil` ID will be returned.

 This method returns `nil` if either the user has opted-out (via iOS) from Ad Tracking, the app itself has limited event usage
 via the `[FBSDKSettings limitEventAndDataUsage]` flag, or a specific Facebook user cannot be identified.
 */
+ (FBSDKGraphRequest *)requestForCustomAudienceThirdPartyIDWithAccessToken:(FBSDKAccessToken *)accessToken;

/*
  Sets a custom user ID to associate with all app events.

 The userID is persisted until it is cleared by passing nil.
 */
+ (void)setUserID:(NSString *)userID;

/*
 Clears the custom user ID to associate with all app events.
 */
+ (void)clearUserID;

/*
  Returns the set custom user ID.
 */
+ (NSString *)userID;

/*
  Sends a request to update the properties for the current user, set by `setUserID:`

 You must call `FBSDKAppEvents setUserID:` before making this call.
 - Parameter properties: the custom user properties
 - Parameter handler: the optional completion handler
 */
+ (void)updateUserProperties:(NSDictionary *)properties handler:(FBSDKGraphRequestHandler)handler;

@end
