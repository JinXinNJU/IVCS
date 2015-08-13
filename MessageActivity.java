import com.sonyericsson.extras.liveware.aef.registration.Registration;
import android.app.Activity;
import android.content.Context;
import android.content.Intent;
import android.os.Bundle;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Button;
import android.widget.Toast;


public final class MessageActivity extends Activity {

    @Override
    public void onCreate(final Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.phonelayout);

        Button btnGlass = (Button) findViewById(R.id.btnglass);
        btnGlass.setOnClickListener(new OnClickListener() {

            @Override
            public void onClick(final View v) {
                startExtension();
            }
        });

        Bundle extras = getIntent().getExtras();
        if (extras != null) {
            String message = extras.getString("Message");
            Toast.makeText(getApplicationContext(), message, Toast.LENGTH_LONG)
                    .show();
        }

        if (MessageExtensionService.Object == null) {
            Intent intent = new Intent(Registration.Intents
                    .EXTENSION_REGISTER_REQUEST_INTENT);
            Context context = getApplicationContext();
            intent.setClass(context, MessageExtensionService.class);
            context.startService(intent);
        }
    }

    public void startExtension() {
        // Check ExtensionService is ready and referenced
        if (MessageExtensionService.Object != null) {
            MessageExtensionService.Object
                    .sendMessageToExtension("Get started");
        }
    }
}
