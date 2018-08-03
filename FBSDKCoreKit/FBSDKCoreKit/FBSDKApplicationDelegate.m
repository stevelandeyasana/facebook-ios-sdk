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

#import "FBSDKApplicationDelegate.h"
#import "FBSDKApplicationDelegate+Internal.h"

#import <objc/runtime.h>

#if !TARGET_OS_TV
#import <SafariServices/SafariServices.h>
#endif

#import "FBSDKAppEvents+Internal.h"
#import "FBSDKConstants.h"
#import "FBSDKDynamicFrameworkLoader.h"
#import "FBSDKError.h"
#import "FBSDKInternalUtility.h"
#import "FBSDKLogger.h"
#import "FBSDKServerConfiguration.h"
#import "FBSDKServerConfigurationManager.h"
#import "FBSDKSettings+Internal.h"
#import "FBSDKUtility.h"

#if !TARGET_OS_TV
#import "FBSDKBoltsMeasurementEventListener.h"
#endif

@implementation FBSDKApplicationDelegate

#pragma mark - Class Methods

+ (instancetype)sharedInstance
{
  static FBSDKApplicationDelegate *_sharedInstance;
  static dispatch_once_t onceToken;
  dispatch_once(&onceToken, ^{
    _sharedInstance = [[self alloc] init];
  });
  return _sharedInstance;
}

#pragma mark - UIApplicationDelegate

- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions
{
  FBSDKAccessToken *cachedToken = [[FBSDKSettings accessTokenCache] fetchAccessToken];
  [FBSDKAccessToken setCurrentAccessToken:cachedToken];
  // fetch app settings
  [FBSDKServerConfigurationManager loadServerConfigurationWithCompletionBlock:NULL];

  if ([[FBSDKSettings autoLogAppEventsEnabled] boolValue]) {
    [self _logSDKInitialize];
  }

  return NO;
}

- (void)applicationDidBecomeActive:(NSNotification *)notification
{
  // Auto log basic events in case autoLogAppEventsEnabled is set
  if ([[FBSDKSettings autoLogAppEventsEnabled] boolValue]) {
    [FBSDKAppEvents activateApp];
  }
}

#pragma mark - Helper Methods

- (void)_logSDKInitialize
{
  NSMutableDictionary *params = [NSMutableDictionary new];
  [params setObject:@1 forKey:@"core_lib_included"];
  [FBSDKAppEvents logEvent:@"fb_sdk_initialize" parameters:params];
}

@end
