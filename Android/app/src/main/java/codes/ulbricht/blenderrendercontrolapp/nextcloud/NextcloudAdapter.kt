package codes.ulbricht.blenderrendercontrolapp.nextcloud

import android.view.View
import android.view.ViewGroup
import android.widget.BaseAdapter
import codes.ulbricht.blenderrendercontrolapp.model.File
import org.jetbrains.anko.*

class NextcloudAdapter : BaseAdapter() {
    private var items: List<File> = ArrayList()

    fun setFiles(items: List<File>) {
        this.items = items

        notifyDataSetChanged()
    }

    override fun getView(position: Int, convertView: View?, parent: ViewGroup?): View {
        val item = getItem(position)

        return with(parent!!.context) {
            frameLayout {
                lparams(width = matchParent, height = wrapContent)
                textView(item.name) {
                    padding = dip(16)
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