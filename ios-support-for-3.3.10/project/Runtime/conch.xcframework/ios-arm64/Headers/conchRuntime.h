#import <UIKit/UIKit.h>
#import <GLKit/GLKit.h>

typedef char* (*HandleFileData)(const char* data, int& len);

@class conchRuntimeImpl;

@interface conchRuntime : NSObject<UIImagePickerControllerDelegate>

#pragma mark - Singleton
+(conchRuntime*)GetIOSConchRuntime;

#pragma mark - Initialization
-(id)initWithFrame:(CGRect)frame URL:(NSString*)pUrl rootViewController:(UIViewController*)rootViewController;
-(void)initConch;
-(void)destroy;

#pragma mark - Lifecycle
-(void)didReceiveMemoryWarning;
-(void)update;

#pragma mark - Touch Events
-(void)touchesBegan:(NSSet *)touches withEvent:(UIEvent *)event;
-(void)touchesMoved:(NSSet *)touches withEvent:(UIEvent *)event;
-(void)touchesEnded:(NSSet *)touches withEvent:(UIEvent *)event;
-(void)touchesCancelled:(NSSet *)touches withEvent:(UIEvent *)event;

#pragma mark - Screen & Orientation
-(void)setScreenOrientation:(int)p_nType;
-(void)onOrientationChanged:(CGSize)size;
-(void)onResize:(CGSize)size;
+(UIInterfaceOrientationMask)getOrientationMask;

#pragma mark - Network
-(int)checkNetworkState;

#pragma mark - JavaScript Bridge
-(void)runJS:(NSString*)script;
-(void)setFileDataHandler:(HandleFileData)func;
-(void)callbackToJSWithClass:(Class)cls methodName:(NSString*)name ret:(NSObject*)retObj;
-(void)callbackToJSWithClassName:(NSString*)cls methodName:(NSString*)name ret:(NSObject*)retObj;
-(void)callbackToJSWithObject:(id)obj methodName:(NSString*)name ret:(NSObject*)retObj;

#pragma mark - UI Components
-(void)showKeyboard:(NSString*)defaultValue 
          maxLength:(int)maxLength 
           multiple:(bool)multiple 
        confirmHold:(bool)confirmHold 
        confirmType:(NSString*)confirmType 
             prompt:(NSString*)prompt 
        promptColor:(NSString*)promptColor 
          inputType:(NSString*)inputType;
-(void)hideKeyboard;
-(void)alert:(NSString*)sInfo;
-(void)chooseImage:(int)count sizeType:(NSString*)sizeType sourceType:(NSString*)sourceType;

#pragma mark - Display
-(void)setDisplayLink:(CADisplayLink*)displayLink;
-(UIView*)getView;

#pragma mark - Internal
-(void)reset;
-(conchRuntimeImpl*)getImpl;
-(void)forceEndAllTouches;
@end
