#import "PTSConfigurationManager.h"

@interface PTSConfigurationManager()
@property (nonatomic, strong) NSDictionary* configurations;
@end

@implementation PTSConfigurationManager
+ (instancetype)sharedObjectWithBundle:(NSBundle *)bundle {
    static PTSConfigurationManager *sharedObject = nil;
    if (!sharedObject) {
        sharedObject = [[self alloc] init];
        [sharedObject initializeWithBundle:bundle];
    }
    return sharedObject;
}

- (void)initializeWithBundle:(NSBundle*)bundle {
    if (!bundle) {
        [self loadBundle:[NSBundle mainBundle]];
    } else {
        [self loadBundle:bundle];
    }
}

- (void)loadBundle:(NSBundle*)bundle {
    NSString *confPath = [bundle pathForResource:@"configuration" ofType:@"json"];
    NSError *err = nil;
    NSData *confData = [NSData dataWithContentsOfFile:confPath];
    //    NSString *rawString = [NSString stringWithContentsOfFile:confPath encoding:NSUTF8StringEncoding error:&err];
    
    self.configurations = [NSJSONSerialization JSONObjectWithData:confData options:0 error:&err];
    
    if (err) {
        CLS_LOG(@"Error when initialize %@ %@", self, err);
        LOG(@"Error when initialize %@ %@", self, err);
    }
}

- (id)configurationForKey:(NSString*)key {
    return [self.configurations objectForKey:key];
}
- (NSInteger)integerConfigurationForKey:(NSString*)key {
    return [[self.configurations objectForKey:key] integerValue];
}
- (BOOL)boolConfigurationForKey:(NSString*)key {
    return [[self.configurations objectForKey:key] boolValue];
}

@end
