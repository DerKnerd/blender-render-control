package codes.ulbricht.blenderrendercontrolapp.nextcloud

import android.app.Activity
import android.os.Bundle
import android.support.design.widget.CoordinatorLayout
import android.view.Menu
import android.view.MenuItem
import android.view.View
import android.widget.RelativeLayout
import android.widget.Toolbar
import codes.ulbricht.blenderrendercontrolapp.R
import codes.ulbricht.blenderrendercontrolapp.model.File
import com.github.kittinunf.fuel.android.extension.responseJson
import com.github.kittinunf.fuel.core.isSuccessful
import com.github.kittinunf.fuel.httpGet
import com.github.kittinunf.fuel.httpPost
import org.jetbrains.anko.*
import org.jetbrains.anko.design.coordinatorLayout
import org.jetbrains.anko.design.themedAppBarLayout
import org.json.JSONObject

class NextcloudActivity : Activity() {
    private val nextcloudView = NextcloudView()

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        nextcloudView.setContentView(this)

        setActionBar(nextcloudView.toolbar)
        actionBar?.setDisplayHomeAsUpEnabled(true)

        nextcloudView.loadData()
    }

    override fun onOptionsItemSelected(item: MenuItem): Boolean {
        when (item.itemId) {
            android.R.id.home -> onBackPressed()
        }

        return super.onOptionsItemSelected(item)
    }

    override fun onCreateOptionsMenu(menu: Menu?): Boolean {
        menu?.apply {
            add(R.string.nextcloud_sync_files).apply {
                tooltipText = getString(R.string.nextcloud_sync_files)

                setShowAsActionFlags(MenuItem.SHOW_AS_ACTION_IF_ROOM)
                setIcon(android.R.drawable.ic_popup_sync)
                setOnMenuItemClickListener {
                    "/nextcloud"
                        .httpPost()
                        .responseJson { request, response, result ->
                            if (response.isSuccessful && response.statusCode == 204) {
                                toast(R.string.toast_sync_started_successfully)
                            } else {
                                val error = if (result.component2() != null) {
                                    result.component2().toString()
                                } else {
                                    result.get().obj().getString("message")
                                }

                                longToast(error)
                            }
                        }

                    true
                }
            }
        }

        return true
    }
}

class NextcloudView : AnkoComponent<NextcloudActivity> {
    private val filesAdapter = NextcloudAdapter()
    lateinit var toolbar: Toolbar

    private lateinit var list: CoordinatorLayout
    private lateinit var spinner: RelativeLayout
    private lateinit var context: AnkoContext<NextcloudActivity>

    fun loadData() {
        list.visibility = View.GONE
        spinner.visibility = View.VISIBLE

        "/nextcloud"
            .httpGet()
            .responseJson { _, response, result ->
                spinner.visibility = View.GONE
                if (response.isSuccessful) {
                    val data = result.get().array()
                    val files = ArrayList<File>()

                    for (i in 0 until data.length()) {
                        val file = data[i] as JSONObject
                        files.add(File(file.getString("path"), file.getString("name"), file.getLong("size")))
                    }

                    filesAdapter.setFiles(files)
                    list.visibility = View.VISIBLE
                } else {
                    val error = if (result.component2() != null) {
                        result.component2().toString()
                    } else {
                        val err = result.get().obj()
                        err.getString("message")
                    }

                    context.longToast(error)
                }
            }
    }

    override fun createView(ui: AnkoContext<NextcloudActivity>): View = with(ui) {
        context = ui

        verticalLayout {
            fitsSystemWindows = true

            themedAppBarLayout(R.style.ThemeOverlay_AppCompat_Dark_ActionBar) {
                toolbar = toolbar {
                    popupTheme = R.style.AppTheme_PopupOverlay
                    titleResource = R.string.nextcloud
                }.lparams(width = matchParent, height = wrapContent)
            }.lparams(width = matchParent, height = wrapContent)

            verticalLayout {
                list = coordinatorLayout {
                    lparams(width = matchParent, height = matchParent)
                    listView {
                        adapter = filesAdapter
                    }.lparams(width = matchParent, height = wrapContent) {
                        leftMargin = dip(16)
                        rightMargin = dip(16)
                    }
                }
                spinner = relativeLayout {
                    lparams(width = matchParent, height = matchParent)

                    themedProgressBar(android.R.attr.progressBarStyle) {
                        visibility = View.VISIBLE
                    }.lparams(width = wrapContent, height = wrapContent) {
                        centerInParent()
                    }
                }
            }
        }
    }
}
