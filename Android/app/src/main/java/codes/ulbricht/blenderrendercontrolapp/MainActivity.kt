package codes.ulbricht.blenderrendercontrolapp

import android.content.Context
import android.graphics.Color
import android.os.Bundle
import android.support.design.widget.AppBarLayout
import android.support.design.widget.NavigationView
import android.support.v4.app.Fragment
import android.support.v4.view.GravityCompat
import android.support.v4.widget.DrawerLayout
import android.support.v7.app.ActionBarDrawerToggle
import android.support.v7.app.AppCompatActivity
import android.support.v7.widget.Toolbar
import android.util.Log
import android.view.MenuItem
import android.view.View
import codes.ulbricht.blenderrendercontrolapp.blender.BlenderAddToRenderQueueFragment
import com.github.kittinunf.fuel.core.FuelManager
import org.jetbrains.anko.*
import org.jetbrains.anko.appcompat.v7.titleResource
import org.jetbrains.anko.appcompat.v7.toolbar
import org.jetbrains.anko.design.appBarLayout
import org.jetbrains.anko.design.coordinatorLayout
import org.jetbrains.anko.design.navigationView
import org.jetbrains.anko.support.v4.drawerLayout

class MainActivity : AppCompatActivity(), NavigationView.OnNavigationItemSelectedListener {
    private lateinit var drawer: DrawerLayout

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        MainView().setContentView(this)

        prepareNavigationDrawer()
        drawer.openDrawer(GravityCompat.START)
    }

    override fun onNavigationItemSelected(menuItem: MenuItem): Boolean {
        menuItem.isChecked = true
        var fragment: Fragment? = null
        val preferences = getSharedPreferences(getString(R.string.preferences_file_key), Context.MODE_PRIVATE)

        when (menuItem.title) {
            getString(R.string.nav_blender_add_to_render_queue) -> fragment = BlenderAddToRenderQueueFragment()
            getString(R.string.nav_other_settings) -> startActivity<SettingsActivity>()
            else -> drawer.openDrawer(GravityCompat.START)
        }

        if (fragment != null) {
            val ft = supportFragmentManager.beginTransaction()
            Log.d(MainActivity::class.java.simpleName, "Starting fragment ${fragment.javaClass.simpleName}")
            ft.replace(R.id.nav_main_view, fragment)
            ft.commit()

            drawer.closeDrawer(GravityCompat.START)
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

        navigationView.menu.apply {
            addSubMenu(R.string.nav_blender).apply {
                add(R.string.nav_blender_start_render_queue)
                add(R.string.nav_blender_pause_render_queue)
                add(R.string.nav_blender_stop_render_queue)
                add(R.string.nav_blender_add_to_render_queue)
            }
            addSubMenu(R.string.nav_nextcloud).apply {
                add(R.string.nav_nextcloud_sync_files)
            }
            addSubMenu(R.string.nav_other).apply {
                add(R.string.nav_other_settings)
            }
        }
    }
}

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
