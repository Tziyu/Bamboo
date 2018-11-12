package com.example.bamboo.util;

import android.content.Context;
import android.content.res.TypedArray;
import android.graphics.Canvas;
import android.graphics.Paint;
import android.util.AttributeSet;
import android.util.Log;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ImageView;

import com.bumptech.glide.Glide;
import com.example.bamboo.R;

import java.util.ArrayList;

/**
 * @author yetote QQ:503779938
 * @name Bamboo
 * @class name：com.example.bamboo.util
 * @class 九宫格布局
 * @time 2018/11/9 13:57
 * @change
 * @chang time
 * @class describe
 */
public class NineImageView extends ViewGroup {
    private ArrayList<String> urlList;
    private Context context;
    private int horizontalViewCount;
    private int[] leftArr;
    private int[] topArr;
    private int[] rightArr;
    private int[] bottomArr;
    private static final String TAG = "NineImageView";
    int size;
    private ImageView[] imageViewArr;
    Paint paint = new Paint();

    void init(Context context, AttributeSet attrs) {
        this.context = context;
        TypedArray ta = context.obtainStyledAttributes(attrs, R.styleable.NineImageView);
        size = ta.getInt(R.styleable.NineImageView_list_size, 0);
        ta.recycle();
        try {
            horizontalViewCount = checkSize(size);
        } catch (Throwable throwable) {
            throwable.printStackTrace();
        }

        leftArr = new int[size];
        topArr = new int[size];
        rightArr = new int[size];
        bottomArr = new int[size];
        imageViewArr = new ImageView[size];
    }

    private int checkSize(int size) throws Throwable {
        if (size == 0) {
            throw new Throwable("列表为空");
        }
        if (size > 9) {
            throw new Throwable("列表最多为9张图片");
        }
        if (size == 1) {
            return 1;
        }
        if (size <= 4) {
            return 2;
        }
        return 3;
    }


    public ArrayList<String> getUrlList() {
        return urlList;
    }

    public void setUrlList(ArrayList<String> urlList) throws Throwable {
        this.urlList = urlList;
    }


    public NineImageView(Context context, AttributeSet attrs) {
        super(context, attrs);
        init(context, attrs);
    }

    public NineImageView(Context context, AttributeSet attrs, int defStyleAttr) {
        super(context, attrs, defStyleAttr);
        init(context, attrs);
    }

    @Override
    protected void onLayout(boolean changed, int l, int t, int r, int b) {
        for (int i = 0; i < getChildCount(); i++) {
            View v = getChildAt(i);
            v.layout(leftArr[i], topArr[i], rightArr[i], bottomArr[i]);
//            imageViewArr[i] = (ImageView) v;
            Glide.with(context).load(urlList.get(i)).into((ImageView) v);
        }
    }

    @Override
    protected void onMeasure(int widthMeasureSpec, int heightMeasureSpec) {
        int selfWidthSpecSize = MeasureSpec.getSize(widthMeasureSpec);
        int childWidthSize = selfWidthSpecSize / horizontalViewCount;
        for (int i = 0; i < getChildCount(); i++) {
            leftArr[i] = (i % horizontalViewCount) * childWidthSize;
            topArr[i] = (i / horizontalViewCount) * childWidthSize;
            rightArr[i] = leftArr[i] + childWidthSize;
            bottomArr[i] = topArr[i] + childWidthSize;
            int childSize = resolveSize(childWidthSize, widthMeasureSpec);
            setMeasuredDimension(childSize, childSize);
        }
    }

}
