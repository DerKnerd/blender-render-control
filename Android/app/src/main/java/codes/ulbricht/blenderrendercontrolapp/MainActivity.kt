package codes.ulbricht.blenderrendercontrolapp

import android.os.Bundle
import android.support.design.widget.NavigationView
import android.support.v4.app.Fragment
import android.support.v4.view.GravityCompat
import android.support.v4.widget.DrawerLayout
import android.support.v7.app.ActionBarDrawerToggle
import android.support.v7.app.AppCompatActivity
import android.support.v7.widget.Toolbar
import android.util.Log
import android.view.MenuItem
import codes.ulbricht.blenderrendercontrolapp.blender.BlenderAddToRenderQueueFragment
import org.jetbrains.anko.find
import org.jetbrains.anko.setContentView

class MainActivity : AppCompatActivity(), NavigationView.OnNavigationItemSelectedListener {
    private lateinit var drawer: DrawerLayout

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        MainView().setContentView(this)
        prepareNavigationDrawer()
    }

    override fun onNavigationItemSelected(menuItem: MenuItem): Boolean {
        menuItem.isChecked = true
        lateinit var fragment: Fragment

        when (menuItem.title) {
            getString(R.string.nav_blender_add_to_render_queue) -> fragment = BlenderAddToRenderQueueFragment()
            else -> drawer.openDrawer(GravityCompat.START)
        }

        val ft = supportFragmentManager.beginTransaction()
        Log.d(MainActivity::class.java.simpleName, "Starting fragment ${fragment.javaClass.simpleName}")
        ft.replace(R.id.nav_main_view, fragment)
        ft.commit()

        drawer.closeDrawer(GravityCompat.START)

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
        }
    }
}