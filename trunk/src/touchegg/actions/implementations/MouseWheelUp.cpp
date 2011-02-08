/**
 * @file /src/touchegg/actions/implementations/MouseWheelUp.cpp
 *
 * @~spanish
 * Este archivo es parte del proyecto Touchégg, usted puede redistribuirlo y/o
 * modificarlo bajo los téminos de la licencia GNU GPL v3.
 *
 * @~english
 * This file is part of the Touchégg project, you can redistribute it and/or
 * modify it under the terms of the GNU GPL v3.
 *
 * @class  MouseWheelUp
 * @author José Expósito
 */
#include "MouseWheelUp.h"

// ************************************************************************** //
// **********              CONSTRUCTORS AND DESTRUCTOR             ********** //
// ************************************************************************** //

MouseWheelUp::MouseWheelUp(const QString& settings) : Action(settings) {
    this->speed = 4;

    QStringList strl = settings.split("=");
    if(strl.length() == 2 && strl.at(0) == "SPEED") {
        bool ok;
        int configSpeed = QString(strl.at(1)).toInt(&ok);
        if(ok && configSpeed >= 1 && configSpeed <= 10)
            this->speed = 11 - configSpeed;
        else
            qWarning() << "Error reading MOUSE_WHELL_UP settings, using the " <<
                    "default settings";
    } else
        qWarning() << "Error reading MOUSE_WHELL_UP settings, using the " <<
                "default settings";
}


// ************************************************************************** //
// **********                    PRIVATE METHODS                   ********** //
// ************************************************************************** //

void MouseWheelUp::scroll() const {
    XEvent event;
    memset(&event, 0x00, sizeof(event));
    event.type = ButtonPress;
    event.xbutton.button = Button4;
    event.xbutton.same_screen = true;

    XQueryPointer(QX11Info::display(), QX11Info::appRootWindow(),
        &event.xbutton.root, &event.xbutton.window,
        &event.xbutton.x_root, &event.xbutton.y_root,
        &event.xbutton.x, &event.xbutton.y,
        &event.xbutton.state);

    event.xbutton.subwindow = event.xbutton.window;

    while(event.xbutton.subwindow) {
        event.xbutton.window = event.xbutton.subwindow;
        XQueryPointer(QX11Info::display(), event.xbutton.window,
            &event.xbutton.root, &event.xbutton.subwindow,&event.xbutton.x_root,
            &event.xbutton.y_root, &event.xbutton.x, &event.xbutton.y,
            &event.xbutton.state);
    }

    XSendEvent(QX11Info::display(), PointerWindow, True, 0xfff, &event);
    XFlush(QX11Info::display());

    usleep(1000);

    event.type = ButtonRelease;
    event.xbutton.state = 0x100;

    XSendEvent(QX11Info::display(), PointerWindow, True, 0xfff, &event);
    XFlush(QX11Info::display());
}


// ************************************************************************** //
// **********                    PUBLIC METHODS                    ********** //
// ************************************************************************** //

void MouseWheelUp::executeStart(const QHash<QString, QVariant>& /*attrs*/) {
    this->scrollCount = 0;
}

void MouseWheelUp::executeUpdate(const QHash<QString, QVariant>& attrs) {
    //qDebug() << attrs.value("velocity y");

    if(this->scrollCount >= MouseWheelUp::speed) {
        if(attrs.contains("delta x") && attrs.contains("delta y"))
            this->scroll();
        this->scrollCount = 0;
    } else {
        this->scrollCount++;
    }
}

void MouseWheelUp::executeFinish(const QHash<QString, QVariant>& /*attrs*/) {}
