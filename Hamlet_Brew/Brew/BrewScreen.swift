//
//  BrewScreen.swift
//  Hamlet_Brew
//
//  Created by Kelvin Kao on 8/2/19.
//  Copyright © 2019 Kelvin Kao. All rights reserved.
//

import Foundation
import UIKit

class BrewScreen: UIView {
    
    static let screenWidth: CGFloat = 128
    static let screenHeight: CGFloat = 146
    
    var menu: BrewMenu? = nil
    
    override init(frame: CGRect) {
        super.init(frame: frame)
        
        backgroundColor = UIColor.white
    }
    
    required init?(coder aDecoder: NSCoder) {
        super.init(coder: aDecoder)
    }
    
    func clearScreen() {
        let allSubviews = subviews
        allSubviews.forEach {
            $0.removeFromSuperview()
        }
    }
    
    func drawImage(literalName: String, atX x: CGFloat, y: CGFloat) {
        let imageView = createImageView(literalName: literalName, positionAtX: x, y: y)
        addSubview(imageView)
    }
    
    func drawTextBox(text: String, atX x: CGFloat, y: CGFloat) {
        let textbox = createTextBox(text: text, positionAtX: x, y: y)
        addSubview(textbox)
    }
    
    func drawBoldLabel(text: String, atX x: CGFloat, y: CGFloat) {
        let label = createBoldLabel(text: text, positionAtX: x, y: y)
        addSubview(label)
    }
    
    func drawLabel(text: String, atX x: CGFloat, y: CGFloat) {
        let label = createLabel(text: text, positionAtX: x, y: y)
        addSubview(label)
    }
    
    func drawMenu(title: String, menuItems: [String], atY y: CGFloat) {
        let menu = createMenu(title: title, menuItems: menuItems, atY: y)
        addSubview(menu)
        self.menu = menu
    }
    
    func changeMenuChoice(key: BrewKeyPad.Key) {
        guard let menu = menu,
            menu.isActive else {
                return
        }
        
        switch key {
        case .arrowUp:
            menu.goToPreviousMenuItem()
        case .arrowDown:
            menu.goToNextMenuItem()
        case .ok:
            menu.commitToCurrentMenuItem()
        default:
            return
        }
    }
}

// MARK: subview components creation

fileprivate extension BrewScreen {
    func createImageView(literalName: String, positionAtX x: CGFloat, y: CGFloat) -> UIImageView {
        let image = UIImage(imageLiteralResourceName: literalName)
        let imageView = UIImageView(image: image)
        imageView.frame = CGRect(x: x, y: y, width: image.size.width, height: image.size.height)
        return imageView
    }
    
    func createTextBox(text: String, positionAtX x: CGFloat, y: CGFloat) -> UILabel {
        let label = createLabel(text: text)
        label.font = UIFont(name: "Helvetica", size: 10.0)
        label.numberOfLines = 5
        label.frame = CGRect(x: x, y: y, width: BrewScreen.screenWidth - 2*x, height: BrewScreen.screenHeight - y)
        return label
    }
    
    func createBoldLabel(text: String, positionAtX x: CGFloat, y: CGFloat) -> UILabel {
        let label = createLabel(text: text)
        label.font = UIFont(name: "Helvetica-Bold", size: 14.0)
        label.frame = CGRect(x: x, y: y, width: label.intrinsicContentSize.width, height: label.intrinsicContentSize.height)
        return label
    }
    
    func createLabel(text: String, positionAtX x: CGFloat, y: CGFloat) -> UILabel {
        let label = createLabel(text: text)
        label.font = UIFont(name: "Helvetica", size: 14.0)
        label.frame = CGRect(x: x, y: y, width: label.intrinsicContentSize.width, height: label.intrinsicContentSize.height)
        return label
    }
    
    func createLabel(text: String) -> UILabel {
        let label = UILabel()
        label.text = text
        label.textAlignment = .left
        label.font = UIFont(name: "Helvetica", size: 14.0)
        return label
    }
    
    func createMenu(title: String, menuItems: [String], atY y: CGFloat) -> BrewMenu {
        let menu = BrewMenu(title: title, menuItems: menuItems)
        menu.frame = CGRect(x: 0.0, y: y, width: BrewScreen.screenWidth, height: BrewScreen.screenHeight - y)
        return menu
    }
}
