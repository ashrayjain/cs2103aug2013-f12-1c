#include <QtWidgets>

#ifndef LAST_COLUMN_DELEGATE_H
#define LAST_COLUMN_DELEGATE_H

class LastColumnDelegate: public QStyledItemDelegate
{
	Q_OBJECT

public:
	LastColumnDelegate(int lastColumn, QObject *parent = 0)
	: QStyledItemDelegate(parent)
	{
		this->_lastColumn = lastColumn;
	}

	void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
	{
		QStyleOptionViewItemV4 opt = option;
		initStyleOption(&opt, index);
		
		if (index.column() == _lastColumn) {//last column
			QStyledItemDelegate::paint(painter, option, index);
			painter->save();

			QString displayText = index.data().toString();
			QTextOption alignment(Qt::AlignRight | Qt::AlignVCenter);
			QPen penHText(QColor("#fff"));
			painter->setPen(penHText);
			QRect rect = opt.rect;
			rect = rect.adjusted(18,0,-18,0);
			painter->drawText(rect, displayText, alignment);

			painter->restore();
		} 
		else 
		{ 
			QStyledItemDelegate::paint(painter, option, index); 
		}
	}

private:
	int _lastColumn;
};
#endif