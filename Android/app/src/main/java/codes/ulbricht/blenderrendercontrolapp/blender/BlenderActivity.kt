package codes.ulbricht.blenderrendercontrolapp.blender

import android.app.Activity
import android.os.Bundle
import android.support.design.widget.CoordinatorLayout
import android.view.Menu
import android.view.MenuItem
import android.view.View
import android.widget.RelativeLayout
import android.widget.Toolbar
import codes.ulbricht.blenderrendercontrolapp.R
import codes.ulbricht.blenderrendercontrolapp.model.QueueEntry
import com.github.kittinunf.fuel.android.extension.responseJson
import com.github.kittinunf.fuel.core.isSuccessful
import com.github.kittinunf.fuel.httpDelete
import com.github.kittinunf.fuel.httpGet
import com.github.kittinunf.fuel.httpPut
import org.jetbrains.anko.*
import org.jetbrains.anko.design.coordinatorLayout
import org.jetbrains.anko.design.themedAppBarLayout
import org.json.JSONArray
import org.json.JSONObject

class BlenderActivity : Activity() {
    private val blenderView = BlenderView()
    private lateinit var startQueue: MenuItem
    private lateinit var pauseQueue: MenuItem
    private lateinit var stopQueue: MenuItem

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        blenderView.setContentView(this)

        setActionBar(blenderView.toolbar)
        actionBar?.setDisplayHomeAsUpEnabled(true)
    }

    private fun loadState() {
        "/blender"
            .httpGet()
            .responseJson { request, response, result ->
                if (response.isSuccessful) {
                    val processing = result.get().obj().getBoolean("processing")
                    startQueue.isVisible = !processing
                    pauseQueue.isVisible = processing
                    stopQueue.isVisible = processing

                    blenderView.updateItems(result.get().obj().getJSONArray("items"))
                }
            }
    }

    override fun onOptionsItemSelected(item: MenuItem): Boolean {
        when (item.itemId) {
            android.R.id.home -> onBackPressed()
        }

        return super.onOptionsItemSelected(item)
    }

    override fun onCreateOptionsMenu(menu: Menu?): Boolean {
        menu?.apply {
            add(R.string.blender_add_to_render_queue).apply {
                tooltipText = getString(R.string.blender_add_to_render_queue)

                setShowAsActionFlags(MenuItem.SHOW_AS_ACTION_IF_ROOM)
                setIcon(android.R.drawable.ic_menu_add)
                setOnMenuItemClickListener {
                    startActivity<BlenderAddToRenderQueueActivity>()
                    true
                }
            }

            startQueue = add(R.string.blender_start_queue).apply {
                tooltipText = getString(R.string.blender_start_queue)
                isVisible = false

                setShowAsActionFlags(MenuItem.SHOW_AS_ACTION_IF_ROOM)
                setIcon(android.R.drawable.ic_media_play)
                setOnMenuItemClickListener {
                    "/blender"
                        .httpPut()
                        .responseString { _, response, result ->
                            if (response.isSuccessful && response.statusCode == 200) {
                                startQueue.isVisible = false
                                pauseQueue.isVisible = true
                                stopQueue.isVisible = true
                                toast(R.string.toast_queue_started_successfully)
                            } else {
                                val error = if (result.component2() != null) {
                                    result.component2().toString()
                                } else {
                                    val err = JSONObject(result.get())
                                    err.getString("message")
                                }

                                longToast(error)
                            }
                        }
                    true
                }
            }
            pauseQueue = add(R.string.blender_pause_queue).apply {
                tooltipText = getString(R.string.blender_pause_queue)
                isVisible = false

                setShowAsActionFlags(MenuItem.SHOW_AS_ACTION_IF_ROOM)
                setIcon(android.R.drawable.ic_media_pause)
                setOnMenuItemClickListener {
                    "/blender"
                        .httpDelete()
                        .responseString { _, response, result ->
                            if (response.isSuccessful && response.statusCode == 200) {
                                startQueue.isVisible = true
                                pauseQueue.isVisible = false
                                stopQueue.isVisible = false
                                toast(R.string.toast_queue_paused_successfully)
                            } else {
                                val error = if (result.component2() != null) {
                                    result.component2().toString()
                                } else {
                                    val err = JSONObject(result.get())
                                    err.getString("message")
                                }

                                longToast(error)
                            }
                        }
                    true
                }
            }
            stopQueue = add(R.string.blender_stop_queue).apply {
                tooltipText = getString(R.string.blender_stop_queue)
                isVisible = false

                setShowAsActionFlags(MenuItem.SHOW_AS_ACTION_IF_ROOM)
                setIcon(R.drawable.ic_stop)
                setOnMenuItemClickListener {
                    "/blender?force=true"
                        .httpDelete()
                        .responseString { _, response, result ->
                            if (response.isSuccessful && response.statusCode == 200) {
                                startQueue.isVisible = true
                                pauseQueue.isVisible = false
                                stopQueue.isVisible = false
                                toast(R.string.toast_queue_stopped_successfully)
                            } else {
                                val error = if (result.component2() != null) {
                                    result.component2().toString()
                                } else {
                                    val err = JSONObject(result.get())
                                    err.getString("message")
                                }

                                longToast(error)
                            }
                        }
                    true
                }
            }
        }

        loadState()

        return true
    }
}

class BlenderView : AnkoComponent<BlenderActivity> {
    private val entryAdapter = BlenderAdapter()
    lateinit var toolbar: Toolbar

    private lateinit var list: CoordinatorLayout
    private lateinit var spinner: RelativeLayout
    private lateinit var context: AnkoContext<BlenderActivity>

    fun updateItems(data: JSONArray) {
        val entries = ArrayList<QueueEntry>()

        for (i in 0 until data.length()) {
            val file = data[i] as JSONObject
            entries.add(QueueEntry(file.getString("path"), file.getInt("width"), file.getInt("height")))
        }

        entryAdapter.setEntries(entries)
        list.visibility = View.VISIBLE
        spinner.visibility = View.GONE
    }

    override fun createView(ui: AnkoContext<BlenderActivity>): View = with(ui) {
        context = ui

        verticalLayout {
            fitsSystemWindows = true

            themedAppBarLayout(R.style.ThemeOverlay_AppCompat_Dark_ActionBar) {
                toolbar = toolbar {
                    id = R.id.toolbar
                    popupTheme = R.style.AppTheme_PopupOverlay
                    titleResource = R.string.blender
                }.lparams(width = matchParent, height = wrapContent)
            }.lparams(width = matchParent, height = wrapContent)

            verticalLayout {
                list = coordinatorLayout {
                    visibility = View.GONE
                    lparams(width = matchParent, height = matchParent)
                    listView {
                        adapter = entryAdapter
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