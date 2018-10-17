/***************************************************************************
                          config.h  -  description
                             -------------------
    begin                : Jun. 22 2015
    copyright            : (C) 2015 by XI'AN JIAOTONG UNIVERSITY
    author               : Cui Bin
 ***************************************************************************/
#ifndef RP_CONFIG_H
#define RP_CONFIG_H

#include <QTreeWidget>
#include <QDomDocument>

class ConfigContent
{

public:
    ConfigContent()
    {}

    ~ConfigContent()
    {}

    void setSpeedrpp(double srpp)
    {
        speed_rpp = srpp;
    }

    double getSpeedrpp()
    {
        return speed_rpp;
    }

    void setSpeedScrweLead(double screwlead)
    {
        speed_screw_lead = screwlead;
    }

    double getPulseEquivalent()
    {
        return  speed_rpp/speed_screw_lead;
    }

    void setLightSpotRadius(double radius)
    {
        light_spot_radius = radius;
    }

    double getLightSpotRadius()
    {
        return  light_spot_radius;
    }

    void setPathInterval(double interval)
    {
        path_interval = interval;
    }

    double getPathInterval()
    {
        return path_interval;
    }

    void setHeightEachLayer(double height)
    {
        height_eachLayer = height;
    }

    double getHeightEachLayer()
    {
        return height_eachLayer;
    }

    void setLaserPowerPWM(double sratio)
    {
        laser_dutyRatio = sratio;
    }

    double getLaserPowerDutyRatio()
    {
        return laser_dutyRatio;
    }

    void setLaserPowerDA(double svoltage)
    {
        laser_DAvoltage = svoltage;
    }

    double getLaserPowervoltage()
    {
        return laser_DAvoltage;
    }

    void setLaserDelay(double offdelay, double ondelay)
    {
        laser_offdelay = offdelay;
        laser_ondelay = ondelay;
    }

    void getLaserDelay(double &offdelay,double &ondelay)
    {
        offdelay = laser_offdelay;
        ondelay = laser_ondelay;
    }

    void setMoveVel(double svel)
    {
        move_Vel = svel;
    }

    double getMoveVel()
    {
        return move_Vel;
    }

    void setMoveAcc(double sacc)
    {
        move_Acc = sacc;
    }

    double getMoveAcc()
    {
        return move_Acc;
    }

    void setOutlineVel(double svel)
    {
        outline_Vel = svel;
    }

    double getOutlineVel()
    {
        return outline_Vel;
    }

    void setFillingVel(double svel)
    {
        filling_Vel = svel;
    }

    double getFillingVel()
    {
        return filling_Vel;
    }

    void setOutlineFillingAcc(double svelacc)
    {
        outlineFilling_Acc = svelacc;
    }

    double getOutlineFillingAcc()
    {
        return outlineFilling_Acc;
    }

    void setSeekOrigin(const double (&pos)[4])
    {
        for(int i = 0; i < 3; i++)
        {
            seekOrigin[i] = pos[i];
        }
    }

    void getSeekOrigin(double &tx,double &ty, double &tz)
    {
        tx = seekOrigin[0];
        ty = seekOrigin[1];
        tz = seekOrigin[2];
    }

    void setOrigin(const double (&pos)[4])
    {
        for(int i = 0; i < 3; i++)
        {
            origin[i] = pos[i];
        }
    }

    void getOrigin(double &tx,double &ty, double &tz)
    {
        tx = origin[0];
        ty = origin[1];
        tz = origin[2];
    }

    void setLeftX(int direction)
    {
        leftX = direction;
    }

    int getLeftX()
    {
        return leftX;
    }

    void setForwardY(int direction)
    {
        forwardY = direction;
    }

    int getForwardY()
    {
        return forwardY;
    }

    void setUpZ(int direction)
    {
        upZ = direction;
    }


    int getUpZ()
    {
        return upZ;
    }

    void setCoordcompensation(bool checked)
    {
        b_Coordcompensation = checked;
    }

    bool getCoordcompensation()
    {
        return b_Coordcompensation;
    }

    void setRotZ(double sv)
    {
        rotZ = sv;
    }

    double getRotZ()
    {
       return rotZ;
    }

    void setTransX(double sv)
    {
        transX = sv;
    }

    double getTransX()
    {
        return transX;
    }

    void setTransY(double sv)
    {
        transY = sv;
    }

    double getTransY()
    {
        return transY;
    }

private:
    double speed_rpp;
    double speed_screw_lead;
    double light_spot_radius;
    double path_interval;
    double height_eachLayer;
    double laser_dutyRatio;
    double laser_DAvoltage;
    double laser_offdelay;
    double laser_ondelay;
    double move_Vel;
    double move_Acc;
    double outline_Vel;
    double filling_Vel;
    double outlineFilling_Acc;
    double origin[3];
    double seekOrigin[3];
    int leftX;
    int forwardY;
    int upZ;
    bool b_Coordcompensation;
    double rotZ;
    double transX;
    double transY;
};


typedef void(*XMLnode_getcallback)(const QDomNode &domnode);
typedef void(*XMLnode_setcallback)(const QDomNode &domnode);

struct XMLNode
{
    XMLnode_getcallback getfunc;
    XMLnode_setcallback setfunc;

    XMLNode()
    {}

    XMLNode(XMLnode_getcallback gfunc,XMLnode_setcallback sfunc):getfunc(gfunc),setfunc(sfunc)
    {}

    void analyse(const QDomNode &domnode) const
    {
        if(getfunc!=NULL)
            this->getfunc(domnode);
    }

    void setNode(const QDomNode &domnode) const
    {
        if(setfunc!=NULL)
            this->setfunc(domnode);
    }

};

class RPConfig : public QTreeWidget
{
    Q_OBJECT
public:
    explicit RPConfig(QWidget *parent = 0);
    ~RPConfig();
    bool readConfig(QIODevice *device);
    bool writeConfig(QIODevice *device);

    static ConfigContent content;

signals:

public slots:

protected:
    void setCallbacks();
    void extract();
    void modify();

    static void extractSpeed(const QDomNode &domnode);
    static void extractLaser(const QDomNode &domnode);
    static void extractMotion(const QDomNode &domnode);
    static void extractMadeUp(const QDomNode &domnode);
    static void extractCoord(const QDomNode &domnode);

    static void recordSpeed(const QDomNode &domnode);
    static void recordLaser(const QDomNode &domnode);
    static void recordMadeUp(const QDomNode &domnode);
    static void recordMotion(const QDomNode &domnode);
    static void recordCoord(const QDomNode &domnode);

private:
    //QList<QTreeWidgetItem *> m_items;
    static QDomDocument domDocument;
    QDomElement m_root;
    QDomNode m_child;
    QHash<QString,XMLNode> list_callback;

};


#endif // RP_CONFIG_H
