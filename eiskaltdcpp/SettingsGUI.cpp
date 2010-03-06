#include "SettingsGUI.h"
#include "WulforSettings.h"
#include "WulforUtil.h"
#include "MainWindow.h"

#include <QListWidgetItem>
#include <QPixmap>
#include <QColor>
#include <QColorDialog>
#include <QStyleFactory>
#include <QFontDialog>
#include <QFileDialog>
#include <QDir>
#include <QFile>

SettingsGUI::SettingsGUI(QWidget *parent) :
    QWidget(parent),
    custom_style(false)
{
    setupUi(this);

    init();
}

SettingsGUI::~SettingsGUI(){

}

void SettingsGUI::init(){
    {//Basic tab
        WulforUtil *WU = WulforUtil::getInstance();
        QStringList styles = QStyleFactory::keys();

        foreach (QString k, styles)
            comboBox_THEMES->addItem(k);

        if (styles.indexOf(WSGET(WS_APP_THEME)) >= 0)
            comboBox_THEMES->setCurrentIndex(styles.indexOf(WSGET(WS_APP_THEME)));

        if (WSGET(WS_APP_FONT).isEmpty()){
            lineEdit_APPFONT->setText(qApp->font().toString());
            WSSET(WS_APP_FONT, qApp->font().toString());
        }
        else
            lineEdit_APPFONT->setText(WSGET(WS_APP_FONT));

        int i = 0;
        foreach (QString f, QDir(QString(CLIENT_TRANSLATIONS_DIR)).entryList(QDir::Files | QDir::NoSymLinks)){
            QString full_path = QString(CLIENT_TRANSLATIONS_DIR) + QDir::separator() + f;
            QString lang = "";

            if (f == "eiskaltdcpp.en.qm")
                lang = tr("English");
            else if (f == "eiskaltdcpp.ru.qm")
                lang = tr("Russian");

            if (!lang.isEmpty()){
                comboBox_LANGS->addItem(lang, full_path);

                if (full_path == WSGET(WS_TRANSLATION_FILE))
                    comboBox_LANGS->setCurrentIndex(i);

                i++;
            }
        }

        lineEdit_LANGFILE->setText(WSGET(WS_TRANSLATION_FILE));

        toolButton_LANGBROWSE->setIcon(WU->getPixmap(WulforUtil::eiFOLDER_BLUE));
    }
    {//Chat tab
        spinBox_PARAGRAPHS->setValue(WIGET(WI_CHAT_MAXPARAGRAPHS));

        checkBox_CHATJOINS->setChecked(WBGET(WB_CHAT_SHOW_JOINS));
        checkBox_CHATHIDDEN->setChecked(WBGET(WB_SHOW_HIDDEN_USERS));
        checkBox_IGNOREPMHUB->setChecked(BOOLSETTING(IGNORE_HUB_PMS));
        checkBox_IGNOREPMBOT->setChecked(BOOLSETTING(IGNORE_BOT_PMS));
        checkBox_REDIRECTPMBOT->setChecked(WBGET(WB_CHAT_REDIRECT_BOT_PMS));
        checkBox_KEEPFOCUS->setChecked(WBGET(WB_CHAT_KEEPFOCUS));

        QColor c;
        QPixmap p(10, 10);

        c.setNamedColor(WSGET(WS_CHAT_LOCAL_COLOR));
        p.fill(c);
        new QListWidgetItem(p, tr("Local user"), listWidget_CHATCOLOR);

        c.setNamedColor(WSGET(WS_CHAT_OP_COLOR));
        p.fill(c);
        new QListWidgetItem(p, tr("Operator"), listWidget_CHATCOLOR);

        c.setNamedColor(WSGET(WS_CHAT_BOT_COLOR));
        p.fill(c);
        new QListWidgetItem(p, tr("Bot"), listWidget_CHATCOLOR);

        c.setNamedColor(WSGET(WS_CHAT_PRIV_LOCAL_COLOR));
        p.fill(c);
        new QListWidgetItem(p, tr("Private: local user"), listWidget_CHATCOLOR);

        c.setNamedColor(WSGET(WS_CHAT_PRIV_USER_COLOR));
        p.fill(c);
        new QListWidgetItem(p, tr("Private: user"), listWidget_CHATCOLOR);

        c.setNamedColor(WSGET(WS_CHAT_SAY_NICK));
        p.fill(c);
        new QListWidgetItem(p, tr("Chat: Say nick"), listWidget_CHATCOLOR);

        c.setNamedColor(WSGET(WS_CHAT_CORE_COLOR));
        p.fill(c);
        new QListWidgetItem(p, tr("Core"), listWidget_CHATCOLOR);

        c.setNamedColor(WSGET(WS_CHAT_STAT_COLOR));
        p.fill(c);
        new QListWidgetItem(p, tr("Status"), listWidget_CHATCOLOR);

        c.setNamedColor(WSGET(WS_CHAT_USER_COLOR));
        p.fill(c);
        new QListWidgetItem(p, tr("User"), listWidget_CHATCOLOR);

        c.setNamedColor(WSGET(WS_CHAT_TIME_COLOR));
        p.fill(c);
        new QListWidgetItem(p, tr("Time stamp"), listWidget_CHATCOLOR);

        c.setNamedColor(WSGET(WS_CHAT_MSG_COLOR));
        p.fill(c);
        new QListWidgetItem(p, tr("Message"), listWidget_CHATCOLOR);

    }

    connect(pushButton_TEST, SIGNAL(clicked()), this, SLOT(slotTestAppTheme()));
    connect(comboBox_THEMES, SIGNAL(activated(int)), this, SLOT(slotThemeChanged()));
    connect(listWidget_CHATCOLOR, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(slotChatColorItemClicked(QListWidgetItem*)));
    connect(toolButton_APPFONTBROWSE, SIGNAL(clicked()), this, SLOT(slotBrowseFont()));
    connect(toolButton_LANGBROWSE, SIGNAL(clicked()), this, SLOT(slotBrowseLng()));
    connect(comboBox_LANGS, SIGNAL(activated(int)), this, SLOT(slotLngIndexChanged(int)));
}

void SettingsGUI::ok(){
    SettingsManager *SM = SettingsManager::getInstance();
    {//Basic tab
        if (custom_style)
            WSSET(WS_APP_THEME, comboBox_THEMES->currentText());
        if (!lineEdit_APPFONT->text().isEmpty())
            WSSET(WS_APP_FONT, lineEdit_APPFONT->text());
        if (!lineEdit_LANGFILE->text().isEmpty())
            WSSET(WS_TRANSLATION_FILE, lineEdit_LANGFILE->text());
    }
    {//Chat tab
        WISET(WI_CHAT_MAXPARAGRAPHS, spinBox_PARAGRAPHS->value());

        WBSET(WB_SHOW_HIDDEN_USERS, checkBox_CHATHIDDEN->isChecked());
        WBSET(WB_CHAT_SHOW_JOINS, checkBox_CHATJOINS->isChecked());
        WBSET(WB_CHAT_REDIRECT_BOT_PMS, checkBox_REDIRECTPMBOT->isChecked());
        WBSET(WB_CHAT_KEEPFOCUS, checkBox_KEEPFOCUS->isChecked());

        int i = 0;

        WSSET(WS_CHAT_LOCAL_COLOR,      QColor(listWidget_CHATCOLOR->item(i++)->icon().pixmap(10, 10).toImage().pixel(0, 0)).name());
        WSSET(WS_CHAT_OP_COLOR,         QColor(listWidget_CHATCOLOR->item(i++)->icon().pixmap(10, 10).toImage().pixel(0, 0)).name());
        WSSET(WS_CHAT_BOT_COLOR,        QColor(listWidget_CHATCOLOR->item(i++)->icon().pixmap(10, 10).toImage().pixel(0, 0)).name());
        WSSET(WS_CHAT_PRIV_LOCAL_COLOR, QColor(listWidget_CHATCOLOR->item(i++)->icon().pixmap(10, 10).toImage().pixel(0, 0)).name());
        WSSET(WS_CHAT_PRIV_USER_COLOR,  QColor(listWidget_CHATCOLOR->item(i++)->icon().pixmap(10, 10).toImage().pixel(0, 0)).name());
        WSSET(WS_CHAT_SAY_NICK,         QColor(listWidget_CHATCOLOR->item(i++)->icon().pixmap(10, 10).toImage().pixel(0, 0)).name());
        WSSET(WS_CHAT_CORE_COLOR,       QColor(listWidget_CHATCOLOR->item(i++)->icon().pixmap(10, 10).toImage().pixel(0, 0)).name());
        WSSET(WS_CHAT_STAT_COLOR,       QColor(listWidget_CHATCOLOR->item(i++)->icon().pixmap(10, 10).toImage().pixel(0, 0)).name());
        WSSET(WS_CHAT_USER_COLOR,       QColor(listWidget_CHATCOLOR->item(i++)->icon().pixmap(10, 10).toImage().pixel(0, 0)).name());
        WSSET(WS_CHAT_TIME_COLOR,       QColor(listWidget_CHATCOLOR->item(i++)->icon().pixmap(10, 10).toImage().pixel(0, 0)).name());
        WSSET(WS_CHAT_MSG_COLOR,        QColor(listWidget_CHATCOLOR->item(i++)->icon().pixmap(10, 10).toImage().pixel(0, 0)).name());

        SM->set(SettingsManager::IGNORE_BOT_PMS, checkBox_IGNOREPMBOT->isChecked());
        SM->set(SettingsManager::IGNORE_HUB_PMS, checkBox_IGNOREPMHUB->isChecked());

        SM->save();
    }

    WulforSettings::getInstance()->save();
}

void SettingsGUI::slotChatColorItemClicked(QListWidgetItem *item){
    QPixmap p(10, 10);
    QColor color(item->icon().pixmap(10, 10).toImage().pixel(0, 0));
    color = QColorDialog::getColor(color);

    if (color.isValid()) {
        p.fill(color);
        item->setIcon(p);
    }
}

void SettingsGUI::slotTestAppTheme(){
    custom_style = true;

    qApp->setStyle(comboBox_THEMES->currentText());
}

void SettingsGUI::slotThemeChanged(){
    custom_style = true;
}

void SettingsGUI::slotBrowseFont(){
    bool ok = false;

    QFont f = QFontDialog::getFont(&ok, this);

    if (ok){
        qApp->setFont(f);
        WSSET(WS_APP_FONT, f.toString());
    }
}

void SettingsGUI::slotBrowseLng(){
    QString file = QFileDialog::getOpenFileName(this, tr("Select translation"), QString(CLIENT_TRANSLATIONS_DIR), tr("Translation (*.qm)"));

    if (!file.isEmpty()){
        WSSET(WS_TRANSLATION_FILE, file);

        WulforSettings::getInstance()->loadTranslation();
        MainWindow::getInstance()->retranslateUi();

        lineEdit_LANGFILE->setText(WSGET(WS_TRANSLATION_FILE));
    }
}

void SettingsGUI::slotLngIndexChanged(int index){
    QString file = comboBox_LANGS->itemData(index).toString();

    WSSET(WS_TRANSLATION_FILE, file);

    WulforSettings::getInstance()->loadTranslation();
    MainWindow::getInstance()->retranslateUi();

    lineEdit_LANGFILE->setText(WSGET(WS_TRANSLATION_FILE));
}
