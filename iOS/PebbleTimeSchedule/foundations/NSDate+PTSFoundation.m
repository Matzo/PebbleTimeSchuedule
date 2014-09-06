#import "NSDate+PTSFoundation.h"

@implementation NSDate (PTSFoundation)
- (NSString*)stringWithFormat:(NSString*)format {
    NSDateFormatter *formatter = [[NSDateFormatter alloc] init];
    
    [formatter setDateFormat:format];
    
    NSString *dateStr = [formatter stringFromDate:self];
    return dateStr;
}
@end
