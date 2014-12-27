#import <UIKit/UIKit.h>
#import <XCTest/XCTest.h>
#import "PTSConfigurationManager.h"

@interface PTSConfigurationManagerTest : XCTestCase

@end

@implementation PTSConfigurationManagerTest

- (void)setUp {
    [super setUp];
    // Put setup code here. This method is called before the invocation of each test method in the class.
}

- (void)tearDown {
    // Put teardown code here. This method is called after the invocation of each test method in the class.
    [super tearDown];
}

- (void)testReadConfiguration {
    PTSConfigurationManager *conf = [PTSConfigurationManager sharedObjectWithBundle:[NSBundle bundleForClass:[self class]]];
    XCTAssertEqualObjects([conf configurationForKey:kConfigurationParseApplicationId], @"test_application_id");
    XCTAssertEqualObjects([conf configurationForKey:kConfigurationParseClientKey], @"test_client_key");
}

@end
