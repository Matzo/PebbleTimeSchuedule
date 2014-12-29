#import <Foundation/Foundation.h>

#define kConfigurationParseApplicationId @"parse_application_id"
#define kConfigurationParseClientKey @"parse_client_key"


#define kPTSUserDefaultsKeyDeviceToken @"kPTSUserDefaultsKeyDeviceToken"



@interface PTSConfigurationManager : NSObject
+ (instancetype)sharedObjectWithBundle:(NSBundle*)bundle;
- (id)configurationForKey:(NSString*)key;
- (NSInteger)integerConfigurationForKey:(NSString*)key;
- (BOOL)boolConfigurationForKey:(NSString*)key;
@end
