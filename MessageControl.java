import android.content.Context;
import android.util.Log;
import com.sony.smarteyeglass.SmartEyeglassControl;
import com.sony.smarteyeglass.extension.util.SmartEyeglassControlUtils;
import com.sonyericsson.extras.liveware.extension.util.control.ControlExtension;
import com.sonyericsson.extras.liveware.extension.util.control.ControlTouchEvent;

public final class MessageControl extends ControlExtension {

    private final SmartEyeglassControlUtils utils;

    private static final int SMARTEYEGLASS_API_VERSION = 1;

    public MessageControl(final Context context,
            final String hostAppPackageName, final String message) {
        super(context, hostAppPackageName);
        utils = new SmartEyeglassControlUtils(hostAppPackageName, null);
        utils.setRequiredApiVersion(SMARTEYEGLASS_API_VERSION);
        utils.activate(context);

        MessageExtensionService.Object.SmartEyeglassControl = this;

        if (message != null) {
            showToast(message);
        } else {
            updateLayout();
        }
    }

    public void requestExtensionStart() {
        startRequest();
    }

    @Override
    public void onResume() {
        updateLayout();
        super.onResume();
    }

    @Override
    public void onDestroy() {
        Log.d(Constants.LOG_TAG, "onDestroy: HControl");
        utils.deactivate();
    };

    @Override
    public void onTouch(final ControlTouchEvent event) {
        super.onTouch(event);
        MessageExtensionService.Object
                .sendMessageToActivity("tapping");
    }

    private void updateLayout() {
        showLayout(R.layout.layout, null);
        sendText(R.id.btn_update_this, "100m");
    }

    public void showToast(final String message) {
        Log.d(Constants.LOG_TAG, "Timeout Dialog : MessageControl");
        utils.showDialogMessage(message,
                SmartEyeglassControl.Intents.DIALOG_MODE_TIMEOUT);
    }
}
