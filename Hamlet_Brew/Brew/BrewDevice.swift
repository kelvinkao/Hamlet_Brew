//
//  BrewDevice.swift
//  Hamlet_Brew
//
//  Created by Kelvin Kao on 8/3/19.
//  Copyright © 2019 Kelvin Kao. All rights reserved.
//

import Foundation
import UIKit

protocol BrewDeviceDelegate {
    func brewDevice(_ : BrewDevice, didPressKey key: BrewKeyPad.Key)
}

class BrewDevice {
    static let deviceWidth: CGFloat = 199.0
    static let deviceTopHeight: CGFloat = 292.0
    static let deviceBottomHeight: CGFloat = 352.0
    
    var delegate: BrewDeviceDelegate? = nil
    
    let brewScreen: BrewScreen
    let brewKeyPad: BrewKeyPad
    
    let topHalfView: UIView
    
    init() {
        brewScreen = BrewScreen(frame: .zero)
        brewKeyPad = BrewKeyPad(frame: .zero)
        topHalfView = BrewDevice.createTopHalfView(screen: brewScreen)
        
        brewKeyPad.delegate = self
    }
}

// MARK: view creation helpers

fileprivate extension BrewDevice {
    class func createTopHalfView(screen: BrewScreen) -> UIView {
        let result = UIView(frame: CGRect(x: 0.0, y: 0.0, width: deviceWidth, height: deviceTopHeight))
        
        let backgroundImage = UIImage(imageLiteralResourceName: "cellTop")
        let background = UIImageView(image: backgroundImage)
        background.frame = CGRect(x: 0.0, y: 0.0, width: BrewDevice.deviceWidth, height: BrewDevice.deviceTopHeight)
        result.addSubview(background)
        
        screen.frame = CGRect(x: 36.0, y: 81.0, width: BrewScreen.screenWidth, height: BrewScreen.screenHeight)
        screen.layer.cornerRadius = 4.0
        result.addSubview(screen)
        
        return result
    }
}

// MARK: BrewKeyPadDelegate

extension BrewDevice: BrewKeyPadDelegate {
    func brewKeyPad(_ : BrewKeyPad, didPressKey key: BrewKeyPad.Key) {
        brewScreen.changeMenuChoice(key: key)
        delegate?.brewDevice(self, didPressKey: key)
    }
}
