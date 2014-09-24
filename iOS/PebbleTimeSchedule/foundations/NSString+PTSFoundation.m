//
//  NSString+PTSFoundation.m
//  PebbleTimeSchedule
//
//  Created by Matsuo Keisuke on 2014/09/24.
//  Copyright (c) 2014年 Matsuo Keisuke. All rights reserved.
//

#import "NSString+PTSFoundation.h"
#import <CommonCrypto/CommonCrypto.h>

@implementation NSString (PTSFoundation)

- (NSString *)md5string {
    const char *data = [self UTF8String];
    if (self.length == 0) {
        return nil;
    }
    CC_LONG len = (CC_LONG)self.length;
    unsigned char result[CC_MD5_DIGEST_LENGTH];
    CC_MD5(data, len, result);
    NSMutableString *ms = @"".mutableCopy;
    for (int i = 0; i < 16; i++) {
        [ms appendFormat:@"%02X",result[i]];
    }
    return ms;
}

@end
