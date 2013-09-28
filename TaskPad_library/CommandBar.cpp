#include "CommandBar.h"

const QStringList CommandBar::COMMAND_LIST = QStringList() \
	<< "add" << "mod" << "del" << "find" << "undo" <<     \
	"redo" << "sync";

const QStringList CommandBar::KEYWORD_LIST = QStringList() \
	<< "name" << "due" << "from" << "to" << "at" << "ppl" \
	<< "note" << "impt" << "rt" << "done" << "undone" <<  \
	"deadline" << "timed" << "floating" << "''";

const QString CommandBar::SPACE = " ";
const QString CommandBar::SINGLE_QUOTATION_MARK = "'";
const QString CommandBar::EMPTY = "";

CommandBar::CommandBar(QWidget *parent)
	:QTextEdit(parent), inputHistory_undo(), inputHistory_redo()
{
	initWidgets();
	initConnections();
}

void CommandBar::initWidgets()
{
	autoCompleteToggle(true);
	initModel();
	initCompleter();
}

void CommandBar::initModel(){
	model = new QStringListModel(this);
	produceCommandModel();
}

void CommandBar::initCompleter()
{
	completer = new QCompleter(this);
	completer->setWidget(this);
	completer->setCompletionMode(QCompleter::InlineCompletion);
	completer->setModel(model);
	completer->setModelSorting(QCompleter::UnsortedModel);
	completer->setCaseSensitivity(Qt::CaseInsensitive);
	completer->setWrapAround(true);
}

void CommandBar::initConnections()
{
	connect(this, SIGNAL(textChanged()), this, SLOT(performCompletion()));
}

QString CommandBar::getCurrentLine()
{
	QTextCursor cursor = textCursor();
	cursor.select(QTextCursor::LineUnderCursor);

	QString result = cursor.selectedText();
	return result;
}

void CommandBar::pushCurrentLine()
{
	QString currentInput = getCurrentLine();
	if(currentInput != EMPTY)
	{
		while(!inputHistory_redo.isEmpty())
		{
			QString input = inputHistory_redo.pop();
			inputHistory_undo.push(input);
		}

		inputHistory_undo.push(currentInput);
		clear();
	}
}

void CommandBar::autoCompleteToggle(bool flag)
{
	autoCompleteFlag = flag;
}

void CommandBar::performCompletion()
{
	produceModel();

	if(autoCompleteFlag)
	{
		QString completionPrefix = getWordUnderCursor();
		if (!completionPrefix.isEmpty() &&
			isLastCharLetter(completionPrefix))
		{
			performCompletion(completionPrefix);
		}
		else//not a word, may be single quotation mark
		{
			if(hasSingleQuotationMark_LHS())
			{
				if(hasSingleQuotationMark_RHS())// case: '<cursor here>' --> '<cursor here>
				{
					clearSingleQuotationMark_RHS();
					insertSpace();
				}
				else// case: '<cursor here> --> '<cursor here>'
				{
					insertSingleQuotationMark_RHS();
				}
			}
		}
	}
}

void CommandBar::performCompletion(const QString &completionPrefix)
{
	if (completionPrefix != completer->completionPrefix())
	{
		completer->setCompletionPrefix(completionPrefix);
	}
	if (completer->completionCount() > 0)
	{
		insertCompletion(completer->currentCompletion());
	}
}

void CommandBar::insertCompletion(const QString &completion)
{
	QTextCursor cursor = textCursor();
	int numberOfCharsToComplete = completion.length() -completer->completionPrefix().length();
	if(numberOfCharsToComplete > 0)// if == 0, nothing to complete
	{
		int insertionPosition = cursor.position();

		TEXT_EDIT_BEGIN
		cursor.insertText(completion.right(numberOfCharsToComplete));
		TEXT_EDIT_END

		cursor.setPosition(insertionPosition);//back to prev. cursor position
		cursor.movePosition(QTextCursor::EndOfWord, QTextCursor::KeepAnchor);
		setTextCursor(cursor);
	}
}

bool CommandBar::isLastCharLetter(QString str)
{
	const int END_INDEX = str.length() - 1;

	bool result = str.at(END_INDEX).isLetter();
	return result;
}

QString CommandBar::getWordUnderCursor()
{
	QTextCursor cursor = textCursor();
	cursor.select(QTextCursor::WordUnderCursor);

	QString completionPrefix = cursor.selectedText();
	return completionPrefix;
}

bool CommandBar::hasSingleQuotationMark(QTextCursor::MoveOperation direction)
{
	bool result;

	QTextCursor cursor = textCursor();
	cursor.movePosition(direction, QTextCursor::KeepAnchor);
	QString str = cursor.selectedText();

	result = str == SINGLE_QUOTATION_MARK;
	return result;
}

bool CommandBar::hasSingleQuotationMark_RHS()
{
	return hasSingleQuotationMark(QTextCursor::Right);
}

bool CommandBar::hasSingleQuotationMark_LHS()
{
	return hasSingleQuotationMark(QTextCursor::Left);
}

void CommandBar::clearSingleQuotationMark_RHS()
{
	QTextCursor cursor = textCursor();
	cursor.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor);

	TEXT_EDIT_BEGIN
	cursor.removeSelectedText();
	TEXT_EDIT_END

	setTextCursor(cursor);
}

void CommandBar::insertSingleQuotationMark_RHS()
{
	QTextCursor cursor = textCursor();
	int insertionPosition = cursor.position();

	TEXT_EDIT_BEGIN
	cursor.insertText(SINGLE_QUOTATION_MARK);
	TEXT_EDIT_END

	cursor.setPosition(insertionPosition);//back to prev. cursor position
	setTextCursor(cursor);
}

void CommandBar::insertSpace()
{
	QTextCursor cursor = textCursor();
	TEXT_EDIT_BEGIN
	cursor.insertText(SPACE);
	TEXT_EDIT_END
}

void CommandBar::produceModel()
{
	if(containsCommand())
	{
		produceKeywordModel();
	}
	else
	{
		produceCommandModel();
	}
}

void CommandBar::produceCommandModel()
{
	model->setStringList(COMMAND_LIST);
}

void CommandBar::produceKeywordModel()
{
	model->setStringList(KEYWORD_LIST);
}

bool CommandBar::containsCommand()
{
	bool result = false;
	QString currentLine = textCursor().block().text();

	for(int i = 0; i < COMMAND_LIST.length(); i++)
	{
		if(currentLine.startsWith(COMMAND_LIST.at(i)))
		{
			result = true;
			break;
		}
	}
	return result;
}

void CommandBar::keyPressEvent(QKeyEvent*event)
{
	bool isHandled = handleKeyPress(event);
	if(!isHandled)
	{
		QTextEdit::keyPressEvent(event);
	}
}

bool CommandBar::handleKeyPress(QKeyEvent*event)
{
	bool isHandled = false;
	switch(event->key())
	{
	case Qt::Key_Escape:
			handleKeyEscape(&isHandled);
		break;
	case Qt::Key_Tab:		// Fallthrough
	case Qt::Key_Space:
			handleKeySpaceAndTab(&isHandled);
		break;
	case Qt::Key_Delete:	// Fallthrough
	case Qt::Key_Backspace:
			handleKeyDeleteAndBackspace();
		break;
	case Qt::Key_Up:
			handleKeyUp();
		break;
	case Qt::Key_Down:
			handleKeyDown();
		break;
	default:
			handleKeyDefault();
		break;
	}
	return isHandled;
}

void CommandBar::handleKeyEscape(bool *isHandled)
{
	QTextCursor cursor = textCursor();
	TEXT_EDIT_BEGIN
	cursor.removeSelectedText();
	TEXT_EDIT_END
	setTextCursor(cursor);
	*isHandled = true;
}

void CommandBar::handleKeySpaceAndTab(bool *isHandled)
{
	QTextCursor cursor = textCursor();
	if(cursor.hasSelection())
	{
		TEXT_EDIT_BEGIN
		cursor.clearSelection();
		cursor.insertText(SPACE);
		TEXT_EDIT_END
		*isHandled = true;
	}
	setTextCursor(cursor);
}

void CommandBar::handleKeyDeleteAndBackspace()
{
	TURN_OFF_AC
	if(hasSingleQuotationMark_LHS() && hasSingleQuotationMark_RHS())
		clearSingleQuotationMark_RHS();
}

void CommandBar::handleKeyUp()
{
	if(!inputHistory_undo.isEmpty())
	{
		QString lastInput = inputHistory_undo.pop();
		inputHistory_redo.push(lastInput);

		clear();
		TEXT_EDIT_BEGIN
		insertHtml(inputHistory_redo.top());
		TEXT_EDIT_END
	}
}

void CommandBar::handleKeyDown()
{
	if(!inputHistory_redo.isEmpty())
	{
		QString prevInput = inputHistory_redo.pop();
		inputHistory_undo.push(prevInput);

		clear();
		if(!inputHistory_redo.isEmpty())
		{
			TEXT_EDIT_BEGIN
			insertHtml(inputHistory_redo.top());
			TEXT_EDIT_END
		}
		else
		{
			insertHtml(EMPTY);
		}
	}
}

void CommandBar::handleKeyDefault()
{
	TURN_ON_AC
}

void CommandBar::mousePressEvent(QMouseEvent*event)
{
	if (autoCompleteFlag) {
		QTextCursor cursor = textCursor();

		TEXT_EDIT_BEGIN
		cursor.removeSelectedText();
		TEXT_EDIT_END

		setTextCursor(cursor);
	}
	QTextEdit::mousePressEvent(event);
}