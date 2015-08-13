import android.content.Context;
import android.content.res.Resources;

public final class ScreenSize {

    private final int width;

    private final int height;

    private final Context context;

    public ScreenSize(final Context context) {
        this.context = context;
        Resources res = context.getResources();
        width = res.getDimensionPixelSize(R.dimen.smarteyeglass_control_width);
        height = res.getDimensionPixelSize(
                R.dimen.smarteyeglass_control_height);
    }

    public Context getContext() {
        return context;
    }

    public int getWidth() {
        return width;
    }

    public int getHeight() {
        return height;
    }

    public boolean equals(final int width, final int height) {
        return this.width == width && this.height == height;
    }
}
