package codes.ulbricht.blenderrendercontrolapp

import android.graphics.Color
import android.support.design.widget.AppBarLayout
import android.support.v4.view.GravityCompat
import android.view.View
import org.jetbrains.anko.*
import org.jetbrains.anko.appcompat.v7.titleResource
import org.jetbrains.anko.appcompat.v7.toolbar
import org.jetbrains.anko.design.appBarLayout
import org.jetbrains.anko.design.coordinatorLayout
import org.jetbrains.anko.design.navigationView
import org.jetbrains.anko.support.v4.drawerLayout

class MainView : AnkoComponent<MainActivity> {
    override fun createView(ui: AnkoContext<MainActivity>): View = with(ui) {
        drawerLayout {
            fitsSystemWindows = true
            id = R.id.drawer_layout
            lparams(width = matchParent, height = matchParent)

            coordinatorLayout {
                lparams(width = matchParent, height = matchParent)

                appBarLayout {
                    toolbar {
                        id = R.id.toolbar
                        popupTheme = R.style.AppTheme_PopupOverlay
                        titleResource = R.string.app_name
                        setTitleTextColor(Color.WHITE)
                    }.lparams(width = matchParent)
                }.lparams(width = matchParent, height = wrapContent) {
                    behavior = AppBarLayout.ScrollingViewBehavior()
                }

                frameLayout {
                    id = R.id.nav_main_view
                    padding = dip(16)
                }.lparams(width = matchParent, height = matchParent) {
                    behavior = AppBarLayout.ScrollingViewBehavior()
                }
            }

            navigationView {
                fitsSystemWindows = true
                id = R.id.nav_view

                setNavigationItemSelectedListener(ui.owner)
            }.lparams(width = wrapContent, height = matchParent, gravity = GravityCompat.START)
        }
    }
}