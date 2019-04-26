package codes.ulbricht.blenderrendercontrolapp.blender

import android.os.Bundle
import android.support.design.widget.CoordinatorLayout
import android.support.v4.app.Fragment
import android.view.Gravity.BOTTOM
import android.view.Gravity.END
import android.view.LayoutInflater
import android.view.View
import android.view.ViewGroup
import android.widget.RelativeLayout
import codes.ulbricht.blenderrendercontrolapp.R
import codes.ulbricht.blenderrendercontrolapp.model.File
import codes.ulbricht.blenderrendercontrolapp.model.QueueEntry
import com.github.kittinunf.fuel.core.isSuccessful
import com.github.kittinunf.fuel.httpGet
import com.github.kittinunf.fuel.httpPost
import org.jetbrains.anko.*
import org.jetbrains.anko.design.coordinatorLayout
import org.jetbrains.anko.design.floatingActionButton
import org.jetbrains.anko.sdk27.coroutines.onClick
import org.jetbrains.anko.support.v4.UI
import org.jetbrains.anko.support.v4.longToast
import org.jetbrains.anko.support.v4.toast
import org.json.JSONArray
import org.json.JSONObject

class BlenderAddToRenderQueueFragment : Fragment() {
    private val filesAdapter = BlenderAddToRenderQueueAdapter()
    private lateinit var list: CoordinatorLayout
    private lateinit var spinner: RelativeLayout

    private fun addToQueue() {
        val selectedFiles = filesAdapter.selectedFiles
        val filesToTransfer = selectedFiles.map {
            QueueEntry(it.path, 7860, 3420)
        }

        "/queue"
            .httpPost()
            .body(JSONArray(filesToTransfer.toTypedArray()).toString())
            .responseString { _, response, _ ->
                if (response.isSuccessful && response.statusCode == 204) {
                    toast(R.string.toast_queue_added_successfully)
                } else {
                    toast(R.string.toast_error_adding_to_queue)
                }
            }
    }

    override fun onViewCreated(view: View, savedInstanceState: Bundle?) {
        super.onViewCreated(view, savedInstanceState)
        list.visibility = View.GONE
        spinner.visibility = View.VISIBLE

        "/nextcloud"
            .httpGet()
            .responseString { _, response, result ->
                spinner.visibility = View.GONE
                if (response.isSuccessful) {
                    val data = JSONArray(result.get())
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
                        val err = JSONObject(result.get())
                        err.getString("message")
                    }

                    longToast(error)
                }
            }
    }

    override fun onCreateView(inflater: LayoutInflater, container: ViewGroup?, savedInstanceState: Bundle?): View? {
        return UI {
            verticalLayout {
                list = coordinatorLayout {
                    lparams(width = matchParent, height = matchParent)
                    listView {
                        adapter = filesAdapter
                    }.lparams(width = matchParent, height = wrapContent) {
                        leftMargin = dip(16)
                        rightMargin = dip(16)
                    }

                    floatingActionButton {
                        imageResource = R.drawable.ic_add
                        onClick {
                            addToQueue()
                        }
                    }.lparams {
                        gravity = BOTTOM or END
                        margin = dip(16)
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
        }.view
    }
}
