package com.example.bamboo;

import android.os.Bundle;
import android.view.View;

import com.example.bamboo.adapter.MainViewPagerAdapter;
import com.example.bamboo.fragment.HomePageFragment;
import com.example.bamboo.fragment.MattersFragment;
import com.example.bamboo.fragment.NearFragment;
import com.example.bamboo.fragment.RecommendFragment;
import com.example.bamboo.util.StatusBarUtils;
import com.google.android.material.tabs.TabLayout;

import java.util.ArrayList;

import androidx.appcompat.app.AppCompatActivity;
import androidx.fragment.app.Fragment;
import androidx.viewpager.widget.ViewPager;

/**
 * @author yetote
 * @decription 起始Activity
 */
public class MainActivity extends AppCompatActivity {
    private TabLayout tabLayout;
    private MainViewPagerAdapter adapter;
    private ArrayList<Fragment> fmList;
    private ArrayList<String> titleList;
    private ViewPager viewPager;
    View statusBar;

    @Override

    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        StatusBarUtils.changedStatusBar(this);

        initView();

        tabLayout.addTab(tabLayout.newTab().setText("推荐"), false);
        tabLayout.addTab(tabLayout.newTab().setText("首页"), true);
        tabLayout.addTab(tabLayout.newTab().setText("附近"), false);
        tabLayout.addTab(tabLayout.newTab().setText("动态"), false);

        fmList = new ArrayList<>();
        fmList.add(new RecommendFragment());
        fmList.add(new HomePageFragment());
        fmList.add(new NearFragment());
        fmList.add(new MattersFragment());

        titleList = new ArrayList<>();
        titleList.add("推荐");
        titleList.add("首页");
        titleList.add("附近");
        titleList.add("动态");

        adapter = new MainViewPagerAdapter(getSupportFragmentManager(), fmList, titleList);
        viewPager.setAdapter(adapter);
        viewPager.setCurrentItem(1);
        tabLayout.setupWithViewPager(viewPager);
    }

    private void initView() {
        tabLayout = findViewById(R.id.main_tabLayout);
        viewPager = findViewById(R.id.main_viewPager);
    }

}

