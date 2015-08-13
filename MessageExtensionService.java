import java.util.List;
import android.content.Intent;
import android.util.Log;
import com.sonyericsson.extras.liveware.aef.control.Control;
import com.sonyericsson.extras.liveware.extension.util.ExtensionService;
import com.sonyericsson.extras.liveware.extension.util.ExtensionUtils;
import com.sonyericsson.extras.liveware.extension.util.control.ControlExtension;
import com.sonyericsson.extras.liveware.extension.util.registration.DeviceInfo;
import com.sonyericsson.extras.liveware.extension.util.registration.DisplayInfo;
import com.sonyericsson.extras.liveware.extension.util.registration.RegistrationAdapter;
import com.sonyericsson.extras.liveware.extension.util.registration.RegistrationInformation;

public final class MessageExtensionService extends ExtensionService {

    public static MessageControl SmartEyeglassControl;
    public static MessageExtensionService Object;
    private static String Message = null;

    public MessageExtensionService() {
        super(Constants.EXTENSION_KEY);
        Object = this;
    }

    @Override
    public void onCreate() {
        super.onCreate();
        Log.d(Constants.LOG_TAG, "onCreate: MessageExtensionService");
    }

    @Override
    protected RegistrationInformation getRegistrationInformation() {
        return new MessageRegistrationInformation(this);
    }

    @Override
    protected boolean keepRunningWhenConnected() {
        return false;
    }

    public void sendMessageToExtension(final String message) {
        Message = message;
        if (SmartEyeglassControl == null) {
            startSmartEyeglassExtension();
        } else {
            SmartEyeglassControl.requestExtensionStart();
        }
    }

    public void startSmartEyeglassExtension() {
        Intent intent = new Intent(Control.Intents
                .CONTROL_START_REQUEST_INTENT);
        ExtensionUtils.sendToHostApp(getApplicationContext(),
                "com.sony.smarteyeglass", intent);
    }

    public void sendMessageToActivity(final String message) {
        Intent intent = new Intent();
        intent.setClass(getBaseContext(), MessageActivity.class);
        intent.addFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
        intent.putExtra("Message", message);
        startActivity(intent);
    }

    @Override
    public ControlExtension createControlExtension(
            final String hostAppPackageName) {
        ScreenSize size = new ScreenSize(this);
        final int width = size.getWidth();
        final int height = size.getHeight();
        List<DeviceInfo> list = RegistrationAdapter.getHostApplication(
                this, hostAppPackageName).getDevices();
        for (DeviceInfo device : list) {
            for (DisplayInfo display : device.getDisplays()) {
                if (display.sizeEquals(width, height)) {
                    return new MessageControl(this,
                            hostAppPackageName, Message);
                }
            }
        }
        throw new IllegalArgumentException("No control for: "
                + hostAppPackageName);
    }
}
