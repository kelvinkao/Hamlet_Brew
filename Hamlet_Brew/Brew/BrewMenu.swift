//
//  BrewMenu.swift
//  Hamlet_Brew
//
//  Created by Kelvin Kao on 8/2/19.
//  Copyright © 2019 Kelvin Kao. All rights reserved.
//

import Foundation
import UIKit

protocol BrewMenuDelegate {
    func brewMenu(_ : BrewMenu, didSelectMenuItem: String)
}

class BrewMenu: UIView {
    let itemHeight: CGFloat = 14.0
    let highlighter: UIView = UIView()
    
    var delegate: BrewMenuDelegate? = nil
    
    let title: String
    let menuItems: [String]
    var currentIndex: Int = 0
    var isActive = false
    
    required init(title: String, menuItems: [String]) {
        self.title = title
        self.menuItems = menuItems
        
        super.init(frame: .zero)
        
        backgroundColor = UIColor.gray
        
        addSubviews()
        highlightCurrentMenuItem()
    }
    
    required init?(coder aDecoder: NSCoder) {
        self.title = ""
        self.menuItems = []
        
        super.init(coder: aDecoder)
    }
    
    func goToNextMenuItem() {
        let newIndex = min(currentIndex + 1, menuItems.count - 1)
        currentIndex = newIndex
        
        highlightCurrentMenuItem()
    }
    
    func goToPreviousMenuItem() {
        let newIndex = max(currentIndex - 1, 0)
        currentIndex = newIndex
        
        highlightCurrentMenuItem()
    }
    
    func commitToCurrentMenuItem() {
        delegate?.brewMenu(self, didSelectMenuItem: menuItems[currentIndex])
    }
    
    func activate() {
        isActive = true
    }
    
    func deactivate() {
        isActive = false
    }
}

fileprivate extension BrewMenu {
    func addSubviews() {
        // title
        let titleLabel = createMenuLabel(text: title)
        titleLabel.font = UIFont(name: "Helvetica-Bold", size: 10.0)
        titleLabel.frame = CGRect(x: 0.0, y: 0.0, width: BrewScreen.screenWidth, height: itemHeight)
        addSubview(titleLabel)
        
        // menu items
        var ySoFar = titleLabel.frame.maxY
        for menuItem in menuItems {
            let menuLabel = createMenuLabel(text: menuItem)
            menuLabel.frame = CGRect(x: 0.0, y: ySoFar, width: BrewScreen.screenWidth, height: itemHeight)
            addSubview(menuLabel)
            
            ySoFar += itemHeight
        }
        
        // highlighter
        highlighter.backgroundColor = UIColor.clear
        highlighter.layer.borderWidth = 1.0
        highlighter.layer.borderColor = UIColor.blue.cgColor
        highlighter.frame = .zero
        addSubview(highlighter)
    }
    
    func createMenuLabel(text: String) -> UILabel {
        let label = UILabel()
        label.font = UIFont(name: "Helvetica", size: 10.0)
        label.text = text
        label.textAlignment = .center
        return label
    }
    
    func highlightCurrentMenuItem() {
        highlighter.frame = CGRect(x: 0.0,
                                   y: CGFloat(currentIndex + 1) * itemHeight,
                                   width: BrewScreen.screenWidth,
                                   height: itemHeight)
    }
}
