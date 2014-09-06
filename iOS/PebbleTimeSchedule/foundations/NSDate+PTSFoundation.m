//
//  NSDate+PTSFoundation.m
//  PebbleTimeSchedule
//
//  Created by Matsuo Keisuke on 2014/09/06.
//  Copyright (c) 2014年 Matsuo Keisuke. All rights reserved.
//

#import "NSDate+PTSFoundation.h"

@implementation NSDate (PTSFoundation)
- (NSString*)stringWithFormat:(NSString*)format {
    NSDateFormatter *formatter = [[NSDateFormatter alloc] init];
    
    [formatter setDateFormat:format];
    
    NSString *dateStr = [formatter stringFromDate:self];
    return dateStr;
}
@end
