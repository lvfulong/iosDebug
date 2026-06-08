#import "SceneDelegate.h"
#import "ViewController.h"

@interface SceneDelegate ()

@end

@implementation SceneDelegate

- (void)scene:(UIScene*)scene willConnectToSession:(UISceneSession*)session options:(UISceneConnectionOptions*)connectionOptions {
     if (![scene isKindOfClass:[UIWindowScene class]]) {
      return;
    }
    self.window = [[UIWindow alloc] initWithWindowScene:(UIWindowScene *)scene];
    ViewController* viewController  = [[ViewController alloc] initWithFrame:[[UIScreen mainScreen] bounds]];
    self.window.rootViewController = viewController;
    [self.window makeKeyAndVisible];
}

- (void)sceneDidDisconnect:(UIScene*)scene {
}

- (void)sceneDidBecomeActive:(UIScene*)scene {
}

- (void)sceneWillResignActive:(UIScene*)scene {
}

- (void)sceneWillEnterForeground:(UIScene*)scene {
}

- (void)sceneDidEnterBackground:(UIScene*)scene {
}

@end
