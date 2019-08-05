//
//  BrewKeyPad.swift
//  Hamlet_Brew
//
//  Created by Kelvin Kao on 8/3/19.
//  Copyright © 2019 Kelvin Kao. All rights reserved.
//

import Foundation
import UIKit

protocol BrewKeyPadDelegate {
    func brewKeyPad(_: BrewKeyPad, didPressKey: BrewKeyPad.Key)
}

class BrewKeyPad: UIView {
    enum Key {
        case unknown
        case arrowUp
        case arrowDown
        case ok
        case num1
        case num2
        case num3
        case num4
        case num5
        case num6
    }
    
    var delegate: BrewKeyPadDelegate? = nil
    let highlightView: HighlightView = HighlightView(frame: .zero)
    var currentKey: Key = .unknown
    
    override init(frame: CGRect) {
        super.init(frame: frame)
        
        addSubviews()
    }
    
    required init?(coder aDecoder: NSCoder) {
        super.init(coder: aDecoder)
    }
    
    func forceHighlightForScreenCapture(key: Key) {
        let verticies = highlightVerticesFor(key: key)
        highlightView.highlight(vertices: verticies)
    }
}

fileprivate extension BrewKeyPad {
    func addSubviews() {
        let backgroundImage = UIImage(imageLiteralResourceName: "cellBottom")
        let background = UIImageView(image: backgroundImage)
        background.frame = CGRect(x: 0.0, y: 0.0, width: BrewDevice.deviceWidth, height: BrewDevice.deviceBottomHeight)
        addSubview(background)
        
        highlightView.frame = CGRect(x: 0.0, y: 0.0, width: BrewDevice.deviceWidth, height: BrewDevice.deviceBottomHeight)
        addSubview(highlightView)
    }
}

extension BrewKeyPad {
    override func touchesBegan(_ touches: Set<UITouch>, with event: UIEvent?) {
        self.setCurrentKeyFrom(touches: touches)
        self.highlightCurrentKey()
    }
    
    override func touchesEnded(_ touches: Set<UITouch>, with event: UIEvent?) {
        setCurrentKeyFrom(touches: touches)
        //highlightView.clearHighlight()
        
        delegate?.brewKeyPad(self, didPressKey: currentKey)
    }
    
    func collideWith(_ point: CGPoint) -> Key {
        let rects = collisionRects
        for (key, rect) in rects {
            if rect.contains(point) {
                return key
            }
        }
        return .unknown
    }
    
    func setCurrentKeyFrom(touches: Set<UITouch>) {
        guard let touch = touches.first else {
            currentKey = .unknown
            return
        }
        
        let collisionResult = collideWith(touch.location(in: self))
        currentKey = collisionResult
    }
    
    var collisionRects: [Key : CGRect] {
        return [ .arrowUp:      CGRect(x: 65.0, y: 99.0, width: 72.0, height: 17.0),
                 .arrowDown:    CGRect(x: 65.0, y: 141.0, width: 72.0, height: 17.0),
                 .ok:           CGRect(x: 80.0, y: 115.0, width: 40.0, height: 28.0),
                 .num1:         CGRect(x: 15.0, y: 189.0, width: 51.0, height: 31.0),
                 .num2:         CGRect(x: 75.0, y: 197.0, width: 51.0, height: 22.0),
                 .num3:         CGRect(x: 133.0, y: 189.0, width: 51.0, height: 31.0),
                 .num4:         CGRect(x: 15.0, y: 216.0, width: 51.0, height: 32.0),
                 .num5:         CGRect(x: 75.0, y: 221.0, width: 51.0, height: 27.0),
                 .num6:         CGRect(x: 133.0, y: 216.0, width: 51.0, height: 32.0)
        ]
    }
    
    func highlightCurrentKey() {
        let verticies = highlightVerticesFor(key: currentKey)
        highlightView.highlight(vertices: verticies)
    }
    
    func highlightVerticesFor(key: Key) -> [CGPoint] {
        switch key {
        case .arrowUp:
            return [ CGPoint(x: 62.0, y: 99.0), CGPoint(x: 81.0, y: 117.0),
                     CGPoint(x: 117.0, y: 117.0), CGPoint(x: 139.0, y: 99.0) ]
        case .arrowDown:
            return [ CGPoint(x: 81.0, y: 139.0), CGPoint(x: 62.0, y: 155.0),
                     CGPoint(x: 139.0, y: 155.0), CGPoint(x: 117.0, y: 139.0) ]
        case .ok:
            return [ CGPoint(x: 82.0, y: 116.0), CGPoint(x: 82.0, y: 139.0),
                     CGPoint(x: 117.0, y: 139.0), CGPoint(x: 117.0, y: 116.0) ]
        case .num1:
            return [ CGPoint(x: 18.0, y: 183.0), CGPoint(x: 18.0, y: 205.0),
                     CGPoint(x: 65.0, y: 216.0), CGPoint(x: 65.0, y: 195.0) ]
        case .num2:
            return [ CGPoint(x: 75.0, y: 197.0), CGPoint(x: 75.0, y: 216.0),
                     CGPoint(x: 126.0, y: 216.0), CGPoint(x: 126.0, y: 197.0) ]
        case .num3:
            return [ CGPoint(x: 135.0, y: 195.0), CGPoint(x: 135.0, y: 216.0),
                     CGPoint(x: 186.0, y: 205.0), CGPoint(x: 186.0, y: 185.0) ]
        case .num4:
            return [ CGPoint(x: 14.0, y: 211.0), CGPoint(x: 18.0, y: 236.0),
                     CGPoint(x: 64.0, y: 246.0), CGPoint(x: 64.0, y: 221.0) ]
        case .num5:
            return [ CGPoint(x: 75.0, y: 222.0), CGPoint(x: 75.0, y: 246.0),
                     CGPoint(x: 125.0, y: 246.0), CGPoint(x: 125.0, y: 222.0) ]
        case .num6:
            return [ CGPoint(x: 135.0, y: 221.0), CGPoint(x: 135.0, y: 246.0),
                     CGPoint(x: 186.0, y: 238.0), CGPoint(x: 184.0, y: 210.0) ]
        default:
            return []
        }
    }
}

// MARK: view to highlight a shape without interacting with touches

class HighlightView: UIView {
    var shapeLayer: CALayer?
    
    override init(frame: CGRect) {
        super.init(frame: frame)
        
        isUserInteractionEnabled = false
    }
    
    required init?(coder aDecoder: NSCoder) {
        super.init(coder: aDecoder)
    }
    
    func highlight(vertices: [CGPoint]) {
        clearHighlight()
        
        let shape = CAShapeLayer()
        layer.addSublayer(shape)
        shape.opacity = 0.5
        shape.fillColor = UIColor.yellow.cgColor
        
        let path = UIBezierPath()
        
        guard let firstPoint = vertices.first else {
            return
        }
        path.move(to: firstPoint)
        
        for point in vertices.dropFirst() {
            path.addLine(to: point)
        }
        
        path.close()
        shape.path = path.cgPath
        
        shapeLayer = shape
    }
    
    func clearHighlight() {
        if let shapeLayer = self.shapeLayer {
            shapeLayer.removeFromSuperlayer()
            self.shapeLayer = nil
        }
    }
}
