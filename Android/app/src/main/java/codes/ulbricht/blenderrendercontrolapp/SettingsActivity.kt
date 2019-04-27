package codes.ulbricht.blenderrendercontrolapp

import android.app.Activity
import android.content.Context
import android.os.Bundle
import android.support.design.widget.TextInputEditText
import android.text.InputType
import android.view.Menu
import android.view.MenuItem
import android.view.View
import android.widget.Toolbar
import com.github.kittinunf.fuel.core.FuelManager
import org.jetbrains.anko.*
import org.jetbrains.anko.design.textInputEditText
import org.jetbrains.anko.design.textInputLayout
import org.jetbrains.anko.design.themedAppBarLayout

class SettingsActivity : Activity() {
    private val settingsView = SettingsView()

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        settingsView.setContentView(this)

        setActionBar(settingsView.toolbar)
        actionBar?.setDisplayHomeAsUpEnabled(true)
    }

    override fun onOptionsItemSelected(item: MenuItem): Boolean {
        when (item.itemId) {
            android.R.id.home -> onBackPressed()
        }
        return super.onOptionsItemSelected(item)
    }

    override fun onCreateOptionsMenu(menu: Menu?): Boolean {
        val preferences = getSharedPreferences(getString(R.string.preferences_file_key), Context.MODE_PRIVATE)
        menu?.apply {
            add(R.string.settings_apply).apply {
                tooltipText = getString(R.string.settings_apply)
                setShowAsActionFlags(MenuItem.SHOW_AS_ACTION_IF_ROOM)
                setIcon(R.drawable.ic_check)
                setOnMenuItemClickListener {
                    val preferencesEditor = preferences.edit()

                    if (settingsView.serverHost != null) {
                        preferencesEditor.putString(
                            getString(R.string.preferences_server_host),
                            settingsView.serverHost?.text.toString()
                        )
                    }

                    if (settingsView.serverPort != null) {
                        preferencesEditor.putString(
                            getString(R.string.preferences_server_port),
                            settingsView.serverPort?.text.toString()
                        )
                    }

                    preferencesEditor.apply()

                    FuelManager.instance.apply {
                        basePath = "http://${preferences.getString(
                            getString(R.string.preferences_server_host),
                            "localhost"
                        )}:${preferences.getString(
                            getString(R.string.preferences_server_port),
                            "1337"
                        )}"
                    }

                    onBackPressed()
                    true
                }
            }
        }

        return true
    }
}

class SettingsView : AnkoComponent<SettingsActivity> {
    var serverHost: TextInputEditText? = null
    var serverPort: TextInputEditText? = null
    lateinit var toolbar: Toolbar

    override fun createView(ui: AnkoContext<SettingsActivity>): View = with(ui) {
        val preferences =
            owner.getSharedPreferences(owner.getString(R.string.preferences_file_key), Context.MODE_PRIVATE)

        verticalLayout {
            lparams(width = matchParent, height = matchParent)

            themedAppBarLayout(R.style.ThemeOverlay_AppCompat_Dark_ActionBar) {
                toolbar = themedToolbar(R.style.Theme_AppCompat) {
                    lparams(width = matchParent)
                    popupTheme = R.style.AppTheme_PopupOverlay
                    titleResource = R.string.settings
                }
            }.lparams(width = matchParent, height = wrapContent)

            verticalLayout {
                padding = dip(16)
                textInputLayout {
                    serverHost = textInputEditText {
                        hintResource = R.string.settings_server_port
                        inputType = InputType.TYPE_CLASS_TEXT

                        setText(preferences.getString(owner.getString(R.string.preferences_server_host), "localhost"))
                    }
                }.lparams(width = matchParent, height = wrapContent)

                textInputLayout {
                    serverPort = textInputEditText {
                        hintResource = R.string.settings_server_host
                        inputType = InputType.TYPE_CLASS_NUMBER

                        setText(preferences.getString(owner.getString(R.string.preferences_server_port), "1337"))
                    }
                }.lparams(width = matchParent, height = wrapContent)
            }
        }
    }
}