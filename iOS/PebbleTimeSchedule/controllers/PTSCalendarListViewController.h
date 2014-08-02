//
//  PTSCalendarListViewController.h
//  PebbleTimeSchedule
//
//  Created by Matsuo Keisuke on 2014/07/01.
//  Copyright (c) 2014年 Matsuo Keisuke. All rights reserved.
//

#import <UIKit/UIKit.h>

@interface PTSCalendarListViewController : UIViewController<
UITableViewDataSource,
UITableViewDelegate
>
@property (nonatomic, weak) IBOutlet UITableView *tableView;
@property (nonatomic, weak) IBOutlet UIButton *sendButton;
@end
