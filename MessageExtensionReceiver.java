import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.util.Log;

public final class MessageExtensionReceiver extends BroadcastReceiver {

    @Override
    public void onReceive(final Context context, final Intent intent) {
        Log.d(Constants.LOG_TAG, "onReceive: " + intent.getAction());
        intent.setClass(context, MessageExtensionService.class);
        context.startService(intent);
    }
}
