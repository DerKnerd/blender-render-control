package codes.ulbricht.blenderrendercontrolapp.blender

import android.os.Bundle
import android.support.v4.app.Fragment
import android.view.LayoutInflater
import android.view.View
import android.view.ViewGroup
import codes.ulbricht.blenderrendercontrolapp.model.File
import com.github.kittinunf.fuel.core.isSuccessful
import com.github.kittinunf.fuel.httpGet
import org.jetbrains.anko.*
import org.jetbrains.anko.support.v4.UI
import org.json.JSONArray
import org.json.JSONObject

class BlenderAddToRenderQueueFragment : Fragment() {
    val filesAdapter = BlenderAddToRenderQueueAdapter()

    fun addToQueue() {
        filesAdapter.selectedFiles
    }

    override fun onViewCreated(view: View, savedInstanceState: Bundle?) {
        super.onViewCreated(view, savedInstanceState)
        "/list-files".httpGet().responseString { _, response, result ->
            if (response.isSuccessful) {
                val data = JSONArray(result.get())
                val files = ArrayList<File>()

                for (i in 0 until data.length()) {
                    val file = data[i] as JSONObject
                    files.add(File(file.getString("path"), file.getString("name"), file.getLong("size")))
                }

                filesAdapter.setFiles(files)
            }
        }
    }

    override fun onCreateView(inflater: LayoutInflater, container: ViewGroup?, savedInstanceState: Bundle?): View? {
        return UI {
            verticalLayout {
                listView {
                    adapter = filesAdapter
                    padding = dip(16)
                }.lparams(width = matchParent, height = wrapContent)
            }
        }.view
    }
}