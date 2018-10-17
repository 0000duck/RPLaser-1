/***************************************************************************
                          config.cpp  -  description
                             -------------------
    begin                : Jun. 22 2015
    copyright            : (C) 2015 by XI'AN JIAOTONG UNIVERSITY
    author               : Cui Bin
 ***************************************************************************/
#include "config.h"
#include <QHeaderView>
#include <QMessageBox>
#include <QDebug>

ConfigContent RPConfig::content = ConfigContent();
QDomDocument RPConfig::domDocument = QDomDocument();

RPConfig::RPConfig(QWidget *parent) : QTreeWidget(parent)
{
    //titles
    QStringList labels;
    labels<<tr("Name")<<tr("Value1")<<tr("Value2")<<tr("Value3");
    this->header()->setSectionResizeMode(QHeaderView::Stretch);
    setHeaderLabels(labels);
    setCallbacks();
}

RPConfig::~RPConfig()
{

}

bool RPConfig::readConfig(QIODevice *device)
{
    QString errorStr;
    int errorLine;
    int errorColumn;

    if(!domDocument.setContent(device,true,&errorStr,&errorLine,&errorColumn))
    {
        QMessageBox::information(window(),tr("Config File"),
        tr("Parse error at line %1, column %2: \n %3").arg(errorLine).arg(errorColumn).arg(errorStr));

        return false;
    }

    m_root = domDocument.documentElement();
    if(m_root.tagName()!="CONFIG")
    {
        QMessageBox::information(window(),tr("Config File"),tr("The file is not a config file."));
        return false;
    }

    clear();
    m_child = m_root.firstChild();

    extract();

    return true;
 }

 bool RPConfig::writeConfig(QIODevice *device)
 {
     modify();
     const int IndentSize = 4;
     QTextStream out(device);
     domDocument.save(out, IndentSize);
     return true;
 }

void RPConfig::extract()
{
    QString tagName;
    QHash<QString,XMLNode>::const_iterator fit;

    while(!m_child.isNull())
    {
        tagName = m_child.nodeName();
        fit = list_callback.find(tagName);

        if(fit != list_callback.end() && fit.key() ==tagName )
        {
            fit.value().analyse(m_child);
        }

        m_child = m_child.nextSibling();
    }
 }

void RPConfig::modify()
{
    QString tagName;
    QHash<QString,XMLNode>::const_iterator fit;

    m_root = domDocument.documentElement();
    m_child = m_root.firstChild();
    while(!m_child.isNull())
    {
        tagName = m_child.nodeName();
        fit = list_callback.find(tagName);

        if(fit != list_callback.end() && fit.key() ==tagName )
        {
            fit.value().setNode(m_child);
        }

        m_child = m_child.nextSibling();
    }
}

void RPConfig::setCallbacks()
{
    list_callback.insert("SPEED",XMLNode(extractSpeed,recordSpeed));
    list_callback.insert("MOTION",XMLNode(extractMotion,recordMotion));
    list_callback.insert("LASER",XMLNode(extractLaser,recordLaser));
    list_callback.insert("MADEUP",XMLNode(extractMadeUp,recordMadeUp));
    list_callback.insert("COORD",XMLNode(extractCoord,recordCoord));

}

void RPConfig::extractSpeed(const QDomNode &domnode)
{
    QDomNodeList children;
    QDomNode child;
    QDomElement ele;
    int listsize;
    double rpp = 0,screwLead = 0;

    children = domnode.childNodes();

    if(children.isEmpty())
    {
        return;
    }

    listsize = children.size();

    for(int i = 0; i < listsize; i++)
    {
        child = children.at(i);
        if(child.nodeName() == "FeedRate")
        {
            ele = child.toElement();
            rpp = ele.attributeNode("round_per_pulse").value().toDouble();
            screwLead = ele.attributeNode("screw_lead").value().toDouble();
        }
    }
    content.setSpeedrpp(rpp);
    content.setSpeedScrweLead(screwLead);
}

void RPConfig::recordSpeed(const QDomNode &domnode)
{
    Q_ASSERT(&domnode!=NULL);
}

void RPConfig::extractLaser(const QDomNode &domnode)
{
    QDomNodeList children;
    QDomNode child;
    QDomElement ele;
    int listsize;
    double lightSpotRadius = 0;
    double laserDutyRatio = 0;
    double laserDAvoltage = 0;
    double laserOffDelay = 0;
    double laserOnDelay = 0;

    children = domnode.childNodes();

    if(children.isEmpty())
    {
        return;
    }

    listsize = children.size();

    for(int i = 0; i < listsize; i++)
    {
        child = children.at(i);

        if(child.nodeName() == "LightSpot")
        {
            ele = child.toElement();
            lightSpotRadius = ele.attributeNode("light_spot_radius").value().toDouble();
        }
        else if(child.nodeName() == "Power")
        {
            ele = child.toElement();
            laserDutyRatio =  ele.attributeNode("PWMduty_ratio").value().toDouble();
            laserDAvoltage =  ele.attributeNode("DAvoltage").value().toDouble();
        }
        else if(child.nodeName() == "Refine")
        {
            ele = child.toElement();
            laserOffDelay =  ele.attributeNode("off_delay").value().toDouble();
            laserOnDelay =  ele.attributeNode("on_delay").value().toDouble();
        }
    }

    content.setLightSpotRadius(lightSpotRadius);
    content.setLaserPowerPWM(laserDutyRatio);
    content.setLaserPowerDA(laserDAvoltage);
    content.setLaserDelay(laserOffDelay,laserOnDelay);
}

void RPConfig::recordLaser(const QDomNode &domnode)
{
    QDomNodeList children;
    QDomNode child;
    QDomElement ele;
    int listsize;
    double offDelay = 0,onDelay = 0;

    children = domnode.childNodes();

    if(children.isEmpty())
    {
        return;
    }

    listsize = children.size();

    for(int i = 0; i < listsize; i++)
    {
        child = children.at(i);

        if(child.nodeName() == "LightSpot")
        {
            ele = child.toElement();
            ele.setAttribute("light_spot_radius",content.getLightSpotRadius());
        }
        else if(child.nodeName() == "Power")
        {
            ele = child.toElement();
            ele.setAttribute("PWMduty_ratio",content.getLaserPowerDutyRatio());
            ele.setAttribute("DAvoltage",content.getLaserPowervoltage());
        }
        else if(child.nodeName() == "Refine")
        {
            ele = child.toElement();
            content.getLaserDelay(offDelay,onDelay);
            ele.setAttribute("off_delay",offDelay);
            ele.setAttribute("on_delay",onDelay);
        }
    }
}

void RPConfig::extractMadeUp(const QDomNode &domnode)
{
    QDomNodeList children;
    QDomNode child;
    QDomElement ele;
    int listsize;
    double pathInterval = 0;
    double heightEachLayer = 0;

    children = domnode.childNodes();

    if(children.isEmpty())
    {
        return;
    }

    listsize = children.size();

    for(int i = 0; i < listsize; i++)
    {
        child = children.at(i);

        if(child.nodeName() == "Path")
        {
            ele = child.toElement();
            pathInterval = ele.attributeNode("path_interval").value().toDouble();
            heightEachLayer = ele.attributeNode("height_each_layer").value().toDouble();
        }
    }

    content.setPathInterval(pathInterval);
    content.setHeightEachLayer(heightEachLayer);
}

void RPConfig::recordMadeUp(const QDomNode &domnode)
{
    QDomNodeList children;
    QDomNode child;
    QDomElement ele;
    int listsize;

    children = domnode.childNodes();

    if(children.isEmpty())
    {
        return;
    }

    listsize = children.size();

    for(int i = 0; i < listsize; i++)
    {
        child = children.at(i);

        if(child.nodeName() == "Path")
        {
            ele = child.toElement();
            ele.setAttribute("path_interval",content.getPathInterval());
            ele.setAttribute("height_each_layer",content.getHeightEachLayer());
        }
    }
}

void RPConfig::extractMotion(const QDomNode &domnode)
{
    QDomNodeList children;
    QDomNode child;
    QDomElement ele;
    int listsize;
    double moveVel = 0;
    double moveAcc = 0;
    double outlineVel = 0;
    double fillingVel = 0;
    double outlineFillingAcc = 0;
    double printOrigin[4];
    double seekOrigin[4];
    int leftx;
    int forwardy;
    int upz;

    children = domnode.childNodes();

    if(children.isEmpty())
    {
        return;
    }

    listsize = children.size();

    for(int i = 0; i < listsize; i++)
    {
        child = children.at(i);

        if(child.nodeName() == "Velocity")
        {
            ele = child.toElement();
            moveVel = ele.attributeNode("move_velocity").value().toDouble();
            moveAcc =  ele.attributeNode("move_acceleration").value().toDouble();
            outlineVel = ele.attributeNode("outline_velocity").value().toDouble();
            fillingVel = ele.attributeNode("filling_velocity").value().toDouble();
            outlineFillingAcc = ele.attributeNode("outline_filling_Acc").value().toDouble();
        }

        else if(child.nodeName() == "OriginSeek")
        {
            ele = child.toElement();
            seekOrigin[0] = ele.attributeNode("origin_seekX").value().toDouble();
            seekOrigin[1]=  ele.attributeNode("origin_seekX").value().toDouble();
            seekOrigin[2] = ele.attributeNode("origin_seekX").value().toDouble();
        }

        else if(child.nodeName() == "Origin")
        {
            ele = child.toElement();
            printOrigin[0] = ele.attributeNode("origin_x").value().toDouble();
            printOrigin[1]=  ele.attributeNode("origin_y").value().toDouble();
            printOrigin[2] = ele.attributeNode("origin_z").value().toDouble();
        }

        else if(child.nodeName() == "MoveDirection")
        {
            ele = child.toElement();
            leftx = ele.attributeNode("left_x").value().toInt();
            forwardy=  ele.attributeNode("forward_y").value().toInt();
            upz = ele.attributeNode("up_z").value().toInt();
        }
    }

    content.setMoveVel(moveVel);
    content.setMoveAcc(moveAcc);
    content.setOutlineVel(outlineVel);
    content.setFillingVel(fillingVel);
    content.setOutlineFillingAcc(outlineFillingAcc);

    content.setSeekOrigin(seekOrigin);
    content.setOrigin(printOrigin);
    content.setLeftX(leftx);
    content.setForwardY(forwardy);
    content.setUpZ(upz);
}

void RPConfig::recordMotion(const QDomNode &domnode)
{
    QDomNodeList children;
    QDomNode child;
    QDomElement ele;
    int listsize;
    double tx,ty,tz;

    children = domnode.childNodes();

    if(children.isEmpty())
    {
        return;
    }

    listsize = children.size();

    for(int i = 0; i < listsize; i++)
    {
        child = children.at(i);

        if(child.nodeName() == "Velocity")
        {
            ele = child.toElement();
            ele.setAttribute("move_velocity",content.getMoveVel());
            ele.setAttribute("move_acceleration",content.getMoveAcc());
            ele.setAttribute("outline_velocity",content.getOutlineVel());
            ele.setAttribute("filling_velocity",content.getFillingVel());
            ele.setAttribute("outline_filling_Acc",content.getOutlineFillingAcc());
        }
        else if(child.nodeName() == "Origin")
        {
            ele = child.toElement();
            content.getOrigin(tx,ty,tz);
            ele.setAttribute("origin_x",tx);
            ele.setAttribute("origin_y",ty);
            ele.setAttribute("origin_z",tz);
        }
    }
}

void RPConfig::extractCoord(const QDomNode &domnode)
{
    QDomNodeList children;
    QDomNode child;
    QDomElement ele;
    int listsize;
    int compensation;
    double rotz = 0;
    double transx = 0;
    double transy = 0;

    children = domnode.childNodes();

    if(children.isEmpty())
    {
        return;
    }

    listsize = children.size();

    for(int i = 0; i < listsize; i++)
    {
        child = children.at(i);

        if(child.nodeName() == "Compensation")
        {
            ele = child.toElement();
            compensation = ele.attributeNode("is_compensation").value().toInt();
            rotz = ele.attributeNode("rot_Z").value().toDouble();
            transx = ele.attributeNode("trans_X").value().toDouble();
            transy = ele.attributeNode("trans_Y").value().toDouble();
        }
    }


    if(compensation == 0)
        content.setCoordcompensation(false);
    else
        content.setCoordcompensation(true);

    content.setRotZ(rotz);
    content.setTransX(transx);
    content.setTransY(transy);

}

void RPConfig::recordCoord(const QDomNode &domnode)
{
    QDomNodeList children;
    QDomNode child;
    QDomElement ele;
    int listsize;
    int compensation;

    if(content.getCoordcompensation())
    {
        compensation = 1;
    }
    else
    {
        compensation = 0;
    }

    children = domnode.childNodes();

    if(children.isEmpty())
    {
        return;
    }

    listsize = children.size();

    for(int i = 0; i < listsize; i++)
    {
        child = children.at(i);

        if(child.nodeName() == "Compensation")
        {
            ele = child.toElement();
            ele.setAttribute("is_compensation",compensation);
            ele.setAttribute("rot_Z",content.getRotZ());
            ele.setAttribute("trans_X",content.getTransX());
            ele.setAttribute("trans_Y",content.getTransY());
        }
    }
}
