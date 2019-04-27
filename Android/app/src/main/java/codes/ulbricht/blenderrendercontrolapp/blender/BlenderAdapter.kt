package codes.ulbricht.blenderrendercontrolapp.blender

import android.view.View
import android.view.ViewGroup
import android.widget.BaseAdapter
import codes.ulbricht.blenderrendercontrolapp.model.QueueEntry
import org.jetbrains.anko.*
import java.io.File

class BlenderAdapter : BaseAdapter() {
    private var items: List<QueueEntry> = ArrayList()

    fun setEntries(items: List<QueueEntry>) {
        this.items = items

        notifyDataSetChanged()
    }

    override fun getView(position: Int, convertView: View?, parent: ViewGroup?): View {
        val item = getItem(position)

        return with(parent!!.context) {
            frameLayout {
                lparams(width = matchParent, height = wrapContent)
                val file = File(item.path)
                textView("${file.nameWithoutExtension} (${item.width} x ${item.height})") {
                    padding = dip(16)
                }
            }
        }
    }

    override fun getItem(position: Int): QueueEntry {
        return items[position]
    }

    override fun getItemId(position: Int): Long {
        return items[position].hashCode().toLong()
    }

    override fun getCount(): Int {
        return items.count()
    }
}