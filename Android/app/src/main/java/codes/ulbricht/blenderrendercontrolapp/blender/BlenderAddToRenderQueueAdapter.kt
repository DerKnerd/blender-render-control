package codes.ulbricht.blenderrendercontrolapp.blender

import android.view.View
import android.view.ViewGroup
import android.widget.BaseAdapter
import codes.ulbricht.blenderrendercontrolapp.model.File
import org.jetbrains.anko.checkBox
import org.jetbrains.anko.frameLayout
import org.jetbrains.anko.matchParent
import org.jetbrains.anko.sdk27.coroutines.onCheckedChange
import org.jetbrains.anko.wrapContent

class BlenderAddToRenderQueueAdapter : BaseAdapter() {
    var items: List<File> = ArrayList()
    val selectedFiles = ArrayList<File>()

    fun setFiles(items: List<File>) {
        this.items = items

        notifyDataSetChanged()
    }

    override fun getView(position: Int, convertView: View?, parent: ViewGroup?): View {
        val item = getItem(position)

        return with(parent!!.context) {
            frameLayout {
                lparams(width = matchParent, height = wrapContent)
                checkBox(item.name) {
                    onCheckedChange { buttonView, isChecked ->
                        if (isChecked) {
                            selectedFiles.add(item)
                        } else {
                            selectedFiles.remove(item)
                        }
                    }
                }
            }
        }
    }

    override fun getItem(position: Int): File {
        return items[position]
    }

    override fun getItemId(position: Int): Long {
        return items[position].hashCode().toLong()
    }

    override fun getCount(): Int {
        return items.count()
    }
}