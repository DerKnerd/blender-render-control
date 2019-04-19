package codes.ulbricht.blenderrendercontrolapp

import android.content.Context
import android.graphics.Color
import android.os.Bundle
import android.support.design.widget.TextInputEditText
import android.support.v7.app.AppCompatActivity
import android.text.InputType
import android.view.View
import com.github.kittinunf.fuel.core.FuelManager
import org.jetbrains.anko.*
import org.jetbrains.anko.appcompat.v7.titleResource
import org.jetbrains.anko.appcompat.v7.toolbar
import org.jetbrains.anko.design.appBarLayout
import org.jetbrains.anko.design.textInputEditText
import org.jetbrains.anko.design.textInputLayout

class SettingsActivity : AppCompatActivity() {
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        SettingsView().setContentView(this)
    }
}

class SettingsView : AnkoComponent<SettingsActivity> {
    private var serverHost: TextInputEditText? = null
    private var serverPort: TextInputEditText? = null

    override fun createView(ui: AnkoContext<SettingsActivity>): View = with(ui) {
        val preferences =
            owner.getSharedPreferences(owner.getString(R.string.preferences_file_key), Context.MODE_PRIVATE)

        verticalLayout {
            lparams(width = matchParent, height = matchParent)

            appBarLayout {
                toolbar {
                    id = R.id.toolbar
                    popupTheme = R.style.AppTheme_PopupOverlay
                    titleResource = R.string.app_name
                    setTitleTextColor(Color.WHITE)

                    menu.apply {
                        add(R.string.settings_apply).apply {
                            tooltipText = owner.getString(R.string.settings_apply)
                            setIcon(R.drawable.ic_check)
                            setOnMenuItemClickListener {
                                val preferencesEditor = preferences.edit()

                                if (serverHost != null) {
                                    preferencesEditor.putString(
                                        owner.getString(R.string.preferences_server_host),
                                        serverHost?.text.toString()
                                    )
                                }

                                if (serverPort != null) {
                                    preferencesEditor.putString(
                                        owner.getString(R.string.preferences_server_port),
                                        serverPort?.text.toString()
                                    )
                                }

                                preferencesEditor.apply()

                                FuelManager.instance.apply {
                                    basePath = "http://${preferences.getString(
                                        owner.getString(R.string.preferences_server_host),
                                        "localhost"
                                    )}:${preferences.getString(
                                        owner.getString(R.string.preferences_server_port),
                                        "1337"
                                    )}"
                                }

                                owner.navigateUpTo(intentFor<MainActivity>())
                            }
                        }
                    }
                }.lparams(width = matchParent)
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