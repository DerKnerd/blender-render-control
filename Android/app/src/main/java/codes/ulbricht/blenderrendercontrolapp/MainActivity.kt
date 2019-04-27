package codes.ulbricht.blenderrendercontrolapp

import android.content.Context
import android.os.Bundle
import android.support.design.widget.NavigationView
import android.support.v4.view.GravityCompat
import android.support.v4.widget.DrawerLayout
import android.support.v7.app.ActionBarDrawerToggle
import android.support.v7.app.AppCompatActivity
import android.support.v7.widget.Toolbar
import android.view.Gravity.BOTTOM
import android.view.Gravity.START
import android.view.MenuItem
import android.view.View
import android.widget.LinearLayout.VERTICAL
import codes.ulbricht.blenderrendercontrolapp.blender.BlenderActivity
import com.github.kittinunf.fuel.core.FuelManager
import org.jetbrains.anko.*
import org.jetbrains.anko.appcompat.v7.titleResource
import org.jetbrains.anko.appcompat.v7.toolbar
import org.jetbrains.anko.design.appBarLayout
import org.jetbrains.anko.design.navigationView
import org.jetbrains.anko.support.v4.drawerLayout

class MainActivity : AppCompatActivity(), NavigationView.OnNavigationItemSelectedListener {
    private lateinit var drawer: DrawerLayout
    private val mainView = MainView()

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        mainView.setContentView(this)

        prepareNavigationDrawer()
        drawer.openDrawer(GravityCompat.START)
    }

    override fun onNavigationItemSelected(menuItem: MenuItem): Boolean {
        val preferences = getSharedPreferences(getString(R.string.preferences_file_key), Context.MODE_PRIVATE)

        when (menuItem.title) {
            getString(R.string.nav_blender_render_queue) -> startActivity<BlenderActivity>()
            getString(R.string.nav_other_settings) -> startActivity(intentFor<SettingsActivity>())
            else -> drawer.openDrawer(GravityCompat.START)
        }

        FuelManager.instance.apply {
            basePath = "http://${preferences.getString(
                getString(R.string.preferences_server_host),
                "localhost"
            )}:${preferences.getString(
                getString(R.string.preferences_server_port),
                "1337"
            )}"
        }

        return true
    }

    private fun prepareNavigationDrawer() {
        val toolbar = find<Toolbar>(R.id.toolbar)
        setSupportActionBar(toolbar)

        drawer = find(R.id.drawer_layout)
        val toggle = ActionBarDrawerToggle(
            this,
            drawer,
            toolbar,
            R.string.navigation_drawer_open,
            R.string.navigation_drawer_close
        )

        drawer.addDrawerListener(toggle)
        toggle.syncState()

        val navigationView = find<NavigationView>(R.id.nav_view)
        navigationView.setNavigationItemSelectedListener(this)
    }
}

class MainView : AnkoComponent<MainActivity> {
    override fun createView(ui: AnkoContext<MainActivity>): View = with(ui) {
        drawerLayout {
            fitsSystemWindows = true
            id = R.id.drawer_layout
            lparams(width = matchParent, height = matchParent)

            verticalLayout {
                appBarLayout {
                    toolbar {
                        id = R.id.toolbar
                        popupTheme = R.style.AppTheme_PopupOverlay
                        titleResource = R.string.app_name
                    }.lparams(width = matchParent, height = wrapContent)
                }.lparams(width = matchParent, height = wrapContent)

                frameLayout {
                    id = R.id.nav_main_view
                }.lparams(width = matchParent, height = matchParent)
            }

            navigationView {
                fitsSystemWindows = true
                id = R.id.nav_view

                setNavigationItemSelectedListener(ui.owner)

                addHeaderView(NavHeaderView().createView(AnkoContext.Companion.create(ctx, this)))

                menu.apply {
                    addSubMenu(R.string.nav_blender).apply {
                        add(R.string.nav_blender_render_queue)
                    }
                    addSubMenu(R.string.nav_nextcloud).apply {
                        add(R.string.nav_nextcloud_files)
                    }
                    addSubMenu(R.string.nav_other).apply {
                        add(R.string.nav_other_settings)
                    }
                }
            }.lparams(width = wrapContent, height = matchParent, gravity = GravityCompat.START)
        }
    }
}

class NavHeaderView : AnkoComponent<NavigationView> {
    override fun createView(ui: AnkoContext<NavigationView>): View = with(ui) {
        themedLinearLayout(R.style.ThemeOverlay_AppCompat_Dark) {
            orientation = VERTICAL
            gravity = BOTTOM
            padding = dimen(R.dimen.nav_header_vertical_spacing)
            backgroundColorResource = R.color.colorPrimary

            imageView(R.mipmap.ic_launcher) {
                topPadding = dimen(R.dimen.nav_header_vertical_spacing)
            }.lparams {
                gravity = START
            }

            textView(R.string.nav_header_blender_render_control) {
                topPadding = dimen(R.dimen.nav_header_vertical_spacing)
                textAppearance = R.style.TextAppearance_AppCompat_Body1
            }
        }
    }
}