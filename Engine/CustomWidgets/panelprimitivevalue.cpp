/*
    RPG Paper Maker Copyright (C) 2017 Marie Laporte

    This file is part of RPG Paper Maker.

    RPG Paper Maker is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    RPG Paper Maker is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Foobar.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "panelprimitivevalue.h"
#include "ui_panelprimitivevalue.h"
#include "wanok.h"

// -------------------------------------------------------
//
//  CONSTRUCTOR / DESTRUCTOR / GET / SET
//
// -------------------------------------------------------

PanelPrimitiveValue::PanelPrimitiveValue(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PanelPrimitiveValue),
    m_model(new PrimitiveValue),
    m_modelNeedDelete(true)
{
    ui->setupUi(this);
}

PanelPrimitiveValue::~PanelPrimitiveValue()
{
    if (m_modelNeedDelete) delete m_model;
    delete ui;
}

PrimitiveValue* PanelPrimitiveValue::model() const { return m_model; }

// -------------------------------------------------------
//
//  INTERMEDIARY FUNCTIONS
//
// -------------------------------------------------------

void PanelPrimitiveValue::initializePrimitives(){
    m_kind = PanelPrimitiveValueKind::Primitives;
    addAnything();
    addNone();
    addNumber();
    addMessage();
    addSwitch();
    addKeyBoard();
    setNumberValue(m_model->numberValue());
    setMessageValue(m_model->messageValue());
    setSwitchValue(m_model->switchValue());
    initialize();
}

// -------------------------------------------------------

void PanelPrimitiveValue::initializeParameterEvent(){
    m_kind = PanelPrimitiveValueKind::ParameterEvent;
    addDefault();
    addAnything();
    addNone();
    addNumber();
    addMessage();
    addSwitch();
    addKeyBoard();
    setNumberValue(m_model->numberValue());
    setMessageValue(m_model->messageValue());
    setSwitchValue(m_model->switchValue());
    initialize();
}

// -------------------------------------------------------

void PanelPrimitiveValue::initializeNumberVariable(){
    m_kind = PanelPrimitiveValueKind::ConstantVariable;
    addNumber();
    addVariable();
    initialize();
    showNumber();
    setNumberValue(m_model->numberValue());
}

// -------------------------------------------------------

void PanelPrimitiveValue::initializeNumber(QStandardItemModel* parameters,
                                           QStandardItemModel* properties)
{
    m_kind = PanelPrimitiveValueKind::Number;
    addParameter(parameters);
    addProperty(properties);
    addNumber();
    addVariable();
    initialize();
    showNumber();
    setNumberValue(m_model->numberValue());
}

// -------------------------------------------------------

void PanelPrimitiveValue::initializeDataBaseCommandId(
        QStandardItemModel* dataBase, QStandardItemModel *parameters,
        QStandardItemModel *properties)
{
    m_kind = PanelPrimitiveValueKind::DataBaseCommandId;
    addDataBase(dataBase);
    addParameter(parameters);
    addProperty(properties);
    addNumber();
    addVariable();
    initialize();
    if (dataBase != nullptr && dataBase->invisibleRootItem()->rowCount() > 0)
        showDataBase();
    else
        showNumber();
    setNumberValue(m_model->numberValue());
}

// -------------------------------------------------------

void PanelPrimitiveValue::initialize(){
    hideAll();
    connect(ui->comboBoxChoice, SIGNAL(currentIndexChanged(int)), this,
            SLOT(on_comboBoxChoiceCurrentIndexChanged(int)));
    setKind(m_model->kind());
}

// -------------------------------------------------------

void PanelPrimitiveValue::initializeModel(PrimitiveValue* m){
    delete m_model;
    m_model = m;
    m_modelNeedDelete = false;
}

// -------------------------------------------------------

void PanelPrimitiveValue::setKind(PrimitiveValueKind kind){
    m_model->setKind(kind);
    ui->comboBoxChoice->setCurrentIndex(getKindIndex(kind));
    updateValue();
}

// -------------------------------------------------------

void PanelPrimitiveValue::setNumberValue(int n){
    m_model->setNumberValue(n);

    switch(m_model->kind()){
    case PrimitiveValueKind::Number:
        ui->spinBoxNumber->setValue(n); break;
    case PrimitiveValueKind::Variable:
        ui->widgetVariable->setCurrentId(n); break;
    case PrimitiveValueKind::Parameter:
        ui->comboBoxParameter->setCurrentIndex(SuperListItem::getIndexById(
                                                   m_model->modelParameter()
                                                   ->invisibleRootItem(), n));
        break;
    case PrimitiveValueKind::Property:
        ui->comboBoxProperty->setCurrentIndex(SuperListItem::getIndexById(
                                                  m_model->modelProperties()
                                                  ->invisibleRootItem(), n));
        break;
    case PrimitiveValueKind::DataBase:
        ui->comboBoxDataBase->setCurrentIndex(SuperListItem::getIndexById(
                                                  m_model->modelDataBase()
                                                  ->invisibleRootItem(), n));
        break;
    case PrimitiveValueKind::KeyBoard:
        ui->comboBoxKeyBoard->setCurrentIndex(
                    SuperListItem::getIndexById(Wanok::get()->project()
                                                ->keyBoardDatas()->model()
                                                ->invisibleRootItem(), n));
        break;
    default:
        break;
    }
}

// -------------------------------------------------------

void PanelPrimitiveValue::setMessageValue(QString m){
    m_model->setMessageValue(m);

    switch(m_model->kind()){
    case PrimitiveValueKind::Message:
        ui->lineEditMessage->setText(m); break;
    case PrimitiveValueKind::Script:
        ui->lineEditScript->setText(m); break;
    default:
        break;
    }
}

// -------------------------------------------------------

void PanelPrimitiveValue::setSwitchValue(bool s){
    m_model->setSwitchValue(s);
    ui->comboBoxSwitch->setCurrentIndex(s ? 0 : 1);
}

// -------------------------------------------------------

void PanelPrimitiveValue::updateValue(){
    switch(m_model->kind()){
    case PrimitiveValueKind::None:
    case PrimitiveValueKind::Default:
    case PrimitiveValueKind::Anything:
        break;
    case PrimitiveValueKind::Number:
        setNumberValue(ui->spinBoxNumber->value()); break;
    case PrimitiveValueKind::Variable:
        setNumberValue(ui->widgetVariable->currentId()); break;
    case PrimitiveValueKind::Parameter:
        setNumberValue(SuperListItem::getIdByIndex(m_model->modelParameter(),
                                                   ui->comboBoxParameter
                                                   ->currentIndex()));
        break;
    case PrimitiveValueKind::Property:
        setNumberValue(SuperListItem::getIdByIndex(m_model->modelProperties(),
                                                   ui->comboBoxProperty
                                                   ->currentIndex()));
        break;
    case PrimitiveValueKind::DataBase:
        setNumberValue(SuperListItem::getIdByIndex(m_model->modelDataBase(),
                                                   ui->comboBoxDataBase
                                                   ->currentIndex()));
        break;
    case PrimitiveValueKind::Message:
        setMessageValue(ui->lineEditMessage->text()); break;
    case PrimitiveValueKind::Script:
        setMessageValue(ui->lineEditScript->text()); break;
    case PrimitiveValueKind::Switch:
        setSwitchValue(ui->comboBoxSwitch->currentIndex() == 0);
        break;
    case PrimitiveValueKind::KeyBoard:
        setNumberValue(SuperListItem::getIdByIndex(
                           Wanok::get()->project()->keyBoardDatas()->model(),
                           ui->comboBoxKeyBoard->currentIndex()));
        break;
    }
}

// -------------------------------------------------------

void PanelPrimitiveValue::hideAll(){
    ui->spinBoxNumber->hide();
    ui->widgetVariable->hide();
    ui->comboBoxParameter->hide();
    ui->comboBoxProperty->hide();
    ui->comboBoxDataBase->hide();
    ui->lineEditMessage->hide();
    ui->lineEditScript->hide();
    ui->comboBoxSwitch->hide();
    ui->comboBoxKeyBoard->hide();
}

// -------------------------------------------------------

void PanelPrimitiveValue::addDefault(){
    ui->comboBoxChoice->addItem("Default", (int)PrimitiveValueKind::Default);
}

// -------------------------------------------------------

void PanelPrimitiveValue::addAnything(){
    ui->comboBoxChoice->addItem("Anything", (int)PrimitiveValueKind::Anything);
}

// -------------------------------------------------------

void PanelPrimitiveValue::addNone(){
    ui->comboBoxChoice->addItem("None", (int)PrimitiveValueKind::None);
}

// -------------------------------------------------------

void PanelPrimitiveValue::addNumber(){
    ui->comboBoxChoice->addItem("Number", (int)PrimitiveValueKind::Number);
}

// -------------------------------------------------------

void PanelPrimitiveValue::addVariable(){
    ui->comboBoxChoice->addItem("Variable", (int)PrimitiveValueKind::Variable);
    ui->widgetVariable->initialize(1);
    connect(ui->widgetVariable->list(), SIGNAL(itemChanged(QListWidgetItem*)),
            this, SLOT(on_variableChanged(QListWidgetItem*)));
}

// -------------------------------------------------------

void PanelPrimitiveValue::addParameter(QStandardItemModel *model){
    if (model != nullptr && model->invisibleRootItem()->rowCount() > 0){
        ui->comboBoxChoice->addItem("Parameter",
                                    (int) PrimitiveValueKind::Parameter);
        m_model->setModelParameter(model);
        SuperListItem::fillComboBox(ui->comboBoxParameter, model);
        connect(ui->comboBoxParameter, SIGNAL(currentIndexChanged(int)), this,
                SLOT(on_comboBoxParameterCurrentIndexChanged(int)));
    }
}

// -------------------------------------------------------

void PanelPrimitiveValue::addProperty(QStandardItemModel* model){
    if (model != nullptr && model->invisibleRootItem()->rowCount() > 0){
        ui->comboBoxChoice->addItem("Property",
                                    (int) PrimitiveValueKind::Property);
        m_model->setModelProperties(model);
        SuperListItem::fillComboBox(ui->comboBoxProperty, model);
        connect(ui->comboBoxProperty, SIGNAL(currentIndexChanged(int)), this,
                SLOT(on_comboBoxPropertyCurrentIndexChanged(int)));
    }
}

// -------------------------------------------------------

void PanelPrimitiveValue::addDataBase(QStandardItemModel *model){
    if (model != nullptr && model->invisibleRootItem()->rowCount() > 0){
        ui->comboBoxChoice->addItem("Selection",
                                    (int) PrimitiveValueKind::DataBase);
        m_model->setModelDataBase(model);
        SuperListItem::fillComboBox(ui->comboBoxDataBase, model);
        connect(ui->comboBoxDataBase, SIGNAL(currentIndexChanged(int)), this,
                SLOT(on_comboBoxDataBaseCurrentIndexChanged(int)));
    }
}

// -------------------------------------------------------

void PanelPrimitiveValue::addMessage(){
    ui->comboBoxChoice->addItem("Message", (int) PrimitiveValueKind::Message);
}

// -------------------------------------------------------

void PanelPrimitiveValue::addScript(){
    ui->comboBoxChoice->addItem("Script", (int) PrimitiveValueKind::Script);
}

// -------------------------------------------------------

void PanelPrimitiveValue::addSwitch(){
    ui->comboBoxChoice->addItem("Switch", (int) PrimitiveValueKind::Switch);
}

// -------------------------------------------------------

void PanelPrimitiveValue::addKeyBoard(){
    ui->comboBoxChoice->addItem("Keyboard", (int) PrimitiveValueKind::KeyBoard);
    SuperListItem::fillComboBox(ui->comboBoxKeyBoard, Wanok::get()->project()
                                ->keyBoardDatas()->model());
    connect(ui->comboBoxKeyBoard, SIGNAL(currentIndexChanged(int)), this,
            SLOT(on_comboBoxKeyBoardCurrentIndexChanged(int)));
}

// -------------------------------------------------------

void PanelPrimitiveValue::showDefault(){
    setKind(PrimitiveValueKind::Default);
    hideAll();
}

// -------------------------------------------------------

void PanelPrimitiveValue::showAnything(){
    setKind(PrimitiveValueKind::Anything);
    hideAll();
}

// -------------------------------------------------------

void PanelPrimitiveValue::showNone(){
    setKind(PrimitiveValueKind::None);
    hideAll();
}

// -------------------------------------------------------

void PanelPrimitiveValue::showNumber(){
    setKind(PrimitiveValueKind::Number);
    hideAll();
    ui->spinBoxNumber->show();
}

// -------------------------------------------------------

void PanelPrimitiveValue::showVariable(){
    setKind(PrimitiveValueKind::Variable);
    hideAll();
    ui->widgetVariable->show();
}

// -------------------------------------------------------

void PanelPrimitiveValue::showParameter(){
    setKind(PrimitiveValueKind::Parameter);
    hideAll();
    ui->comboBoxParameter->show();
}

// -------------------------------------------------------

void PanelPrimitiveValue::showProperty(){
    setKind(PrimitiveValueKind::Property);
    hideAll();
    ui->comboBoxProperty->show();
}

// -------------------------------------------------------

void PanelPrimitiveValue::showDataBase(){
    setKind(PrimitiveValueKind::DataBase);
    hideAll();
    ui->comboBoxDataBase->show();
}

// -------------------------------------------------------

void PanelPrimitiveValue::showMessage(){
    setKind(PrimitiveValueKind::Message);
    hideAll();
    ui->lineEditMessage->show();
}

// -------------------------------------------------------

void PanelPrimitiveValue::showScript(){
    setKind(PrimitiveValueKind::Script);
    hideAll();
    ui->lineEditScript->show();
}

// -------------------------------------------------------

void PanelPrimitiveValue::showSwitch(){
    setKind(PrimitiveValueKind::Switch);
    hideAll();
    ui->comboBoxSwitch->show();
}

// -------------------------------------------------------

void PanelPrimitiveValue::showKeyBoard(){
    setKind(PrimitiveValueKind::KeyBoard);
    hideAll();
    ui->comboBoxKeyBoard->show();
}

// -------------------------------------------------------

int PanelPrimitiveValue::getKindIndex(PrimitiveValueKind kind){
    for (int i = 0; i < ui->comboBoxChoice->count(); i++){
        if (ui->comboBoxChoice->itemData(i).toInt() == (int) kind)
            return i;
    }

    return -1;
}

// -------------------------------------------------------

void PanelPrimitiveValue::initializeCommand(EventCommand* command, int &i){
    switch (m_kind) {
    case PanelPrimitiveValueKind::Primitives:
    case PanelPrimitiveValueKind::ParameterEvent:
        break;
    case PanelPrimitiveValueKind::ConstantVariable:
    case PanelPrimitiveValueKind::DataBaseCommandId:
    case PanelPrimitiveValueKind::Number:
        setKind(static_cast<PrimitiveValueKind>(command
                                                ->valueCommandAt(i++).toInt()));
        setNumberValue(command->valueCommandAt(i++).toInt());
        break;
    }
}

// -------------------------------------------------------

void PanelPrimitiveValue::getCommand(QVector<QString> &command){
    switch (m_kind) {
    case PanelPrimitiveValueKind::Primitives:
    case PanelPrimitiveValueKind::ParameterEvent:
        break;
    case PanelPrimitiveValueKind::ConstantVariable:
    case PanelPrimitiveValueKind::DataBaseCommandId:
    case PanelPrimitiveValueKind::Number:
        command.append(QString::number((int)m_model->kind()));
        command.append(QString::number(m_model->numberValue()));
        break;
    }
}


// -------------------------------------------------------
//
//  EVENTS
//
// -------------------------------------------------------

void PanelPrimitiveValue::on_comboBoxChoiceCurrentIndexChanged(int index){
    PrimitiveValueKind kind = static_cast<PrimitiveValueKind>(ui->comboBoxChoice
                                                              ->itemData(index)
                                                              .toInt());

    switch(kind){
    case PrimitiveValueKind::Default:
        showDefault(); break;
    case PrimitiveValueKind::Anything:
        showAnything(); break;
    case PrimitiveValueKind::None:
        showNone(); break;
    case PrimitiveValueKind::Number:
        showNumber(); break;
    case PrimitiveValueKind::Variable:
        showVariable(); break;
    case PrimitiveValueKind::Parameter:
        showParameter(); break;
    case PrimitiveValueKind::Property:
        showProperty(); break;
    case PrimitiveValueKind::DataBase:
        showDataBase(); break;
    case PrimitiveValueKind::Message:
        showMessage(); break;
    case PrimitiveValueKind::Script:
        showScript(); break;
    case PrimitiveValueKind::Switch:
        showSwitch(); break;
    case PrimitiveValueKind::KeyBoard:
        showKeyBoard(); break;
    }
}

// -------------------------------------------------------

void PanelPrimitiveValue::on_spinBoxNumber_valueChanged(int i){
    setNumberValue(i);
}

// -------------------------------------------------------

void PanelPrimitiveValue::on_variableChanged(QListWidgetItem*){
    setNumberValue(ui->widgetVariable->currentId());
}

// -------------------------------------------------------

void PanelPrimitiveValue::on_comboBoxParameterCurrentIndexChanged(int index){
    setNumberValue(SuperListItem::getIdByIndex(m_model->modelParameter(),
                                               index));
}

// -------------------------------------------------------

void PanelPrimitiveValue::on_comboBoxPropertyCurrentIndexChanged(int index){
    setNumberValue(SuperListItem::getIdByIndex(m_model->modelProperties(),
                                               index));
}

// -------------------------------------------------------

void PanelPrimitiveValue::on_comboBoxDataBaseCurrentIndexChanged(int index){
    setNumberValue(SuperListItem::getIdByIndex(m_model->modelDataBase(),
                                               index));
}

// -------------------------------------------------------

void PanelPrimitiveValue::on_comboBoxKeyBoardCurrentIndexChanged(int index){
    setNumberValue(SuperListItem::getIdByIndex(Wanok::get()->project()
                                               ->keyBoardDatas()->model(),
                                               index));
}

// -------------------------------------------------------

void PanelPrimitiveValue::on_lineEditMessage_textChanged(const QString &text){
    setMessageValue(text);
}

// -------------------------------------------------------

void PanelPrimitiveValue::on_lineEditScript_textChanged(const QString &text){
    setMessageValue(text);
}

// -------------------------------------------------------

void PanelPrimitiveValue::on_comboBoxSwitch_currentIndexChanged(int index){
    setSwitchValue(index == 0);
}
