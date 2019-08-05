//
//  ViewController.swift
//  Hamlet_Brew
//
//  Created by Kelvin Kao on 8/1/19.
//  Copyright © 2019 Kelvin Kao. All rights reserved.
//

import UIKit

class ViewController: UIViewController {
    let brewDevice = BrewDevice()
    let gameLogic: GameLogic
    
    required init?(coder aDecoder: NSCoder) {
        gameLogic = GameLogic(device: brewDevice)
        
        super.init(coder: aDecoder)
    }
    
    override func viewDidLoad() {
        super.viewDidLoad()
        
        view.backgroundColor = UIColor.white
        
        addSubviews(forScreenCapture: false)
    }
    
    override func viewDidAppear(_ animated: Bool) {
        super.viewDidAppear(animated)
        
        gameLogic.startInInitialState()
    }
}

fileprivate extension ViewController {
    func addSubviews(forScreenCapture: Bool) {
        let topHalfView = brewDevice.topHalfView
        topHalfView.frame = CGRect(x: 0.0,
                                   y: 50.0,
                                   width: BrewDevice.deviceWidth,
                                   height: BrewDevice.deviceTopHeight)
        view.addSubview(topHalfView)
        
        let keyPad = brewDevice.brewKeyPad
        if forScreenCapture {
            keyPad.frame = CGRect(x: topHalfView.frame.maxX + 2.0,
                                  y: topHalfView.frame.minY + BrewDevice.deviceTopHeight - BrewDevice.deviceBottomHeight,
                                  width: BrewDevice.deviceWidth,
                                  height: BrewDevice.deviceBottomHeight)
        } else {
            keyPad.frame = CGRect(x: 0.0,
                                  y: topHalfView.frame.maxY,
                                  width: BrewDevice.deviceWidth,
                                  height: BrewDevice.deviceBottomHeight)
        }
        view.addSubview(keyPad)
        
        if forScreenCapture {
            let goButton = createGoButton()
            goButton.frame = CGRect(x: 0.0, y: keyPad.frame.maxY + 30.0, width: 50.0, height: 50.0)
            view.addSubview(goButton)
            
            let cueLabel = configuredCueLabel()
            cueLabel.frame = CGRect(x: goButton.frame.maxX + 15.0, y: goButton.frame.minY, width: view.frame.size.width - goButton.frame.maxX - 15.0, height: goButton.frame.size.height)
            view.addSubview(cueLabel)
            
            displayCue(at: ViewController.cueIndex)
        }
    }
}

// MARK: special UI to queue / cue button interactions for screen recording

extension ViewController {
    enum Cue: String, CaseIterable {
        case press1 = "Press 1 for outdoor"
        case press2 = "Press 2 for outdoor"
        case press3 = "Press 3 for outdoor"
        case press4 = "Press 4 for wall"
        case press5 = "Press 5 for wall"
        case press6 = "Press 6 for wall"
        case pressOkForPolonius = "Press OK for Polonius"
        case backToMenuForKenny = "Reset for Kenny"
        case pressDownForKenny = "Press Down for Kenny"
        case pressOkForKenny = "Press OK for Kenny"
        case backToMenuForSplinter = "Reset for Splinter"
        case pressDownToKennyForSplinter = "Press Down to Kenny for Splinter"
        case pressDownForSplinter = "Press Down for Splinter"
        case pressOkForSplinter = "Press OK for Splinter"
    }
    
    func createGoButton() -> UIButton {
        let button = UIButton(type: .custom)
        button.backgroundColor = UIColor.yellow
        button.setTitle("Go", for: .normal)
        button.setTitleColor(UIColor.green, for: .normal)
        button.addTarget(self, action: #selector(goButtonTapped), for: .touchUpInside)
        return button
    }
    
    static var cueIndex: Int = 0
    static var cueLabel: UILabel = UILabel()
    
    func configuredCueLabel() -> UILabel {
        let label = ViewController.cueLabel
        label.textAlignment = .left
        label.font = UIFont(name: "Helvetica", size: 12.0)
        return label
    }
    
    @objc func goButtonTapped() {
        triggerCue(at: ViewController.cueIndex)
        ViewController.cueIndex += 1
        displayCue(at: ViewController.cueIndex)
    }
    
    func triggerCue(at index: Int) {
        guard let theCue = cue(at: index) else {
            return
        }
        
        switch theCue {
        case .press1:
            cueKey(.num1)
        case .press2:
            cueKey(.num2)
        case .press3:
            cueKey(.num3)
        case .press4:
            cueKey(.num4)
        case .press5:
            cueKey(.num5)
        case .press6:
            cueKey(.num6)
        case .pressOkForPolonius:
            cueKey(.ok)
        case .backToMenuForKenny:
            gameLogic.forceBackToMenuForScreenCapture()
        case .pressDownForKenny:
            cueKey(.arrowDown)
        case .pressOkForKenny:
            cueKey(.ok)
        case .backToMenuForSplinter:
            gameLogic.forceBackToMenuForScreenCapture()
        case .pressDownToKennyForSplinter:
            cueKey(.arrowDown)
        case .pressDownForSplinter:
            cueKey(.arrowDown)
        case .pressOkForSplinter:
            cueKey(.ok)
        }
    }
    
    func displayCue(at index: Int) {
        guard let theCue = cue(at: index) else {
            ViewController.cueLabel.text = "(N/A)"
            return
        }
        
        ViewController.cueLabel.text = "Up Next: \(theCue.rawValue)"
    }
    
    func cue(at index: Int) -> Cue? {
        return (index < Cue.allCases.count) ? cueList[index] : nil
    }
    
    var cueList: [Cue] {
        return Cue.allCases
    }
    
    func cueKey(_ key: BrewKeyPad.Key) {
        brewDevice.brewKeyPad(brewDevice.brewKeyPad, didPressKey: key)
        brewDevice.brewKeyPad.forceHighlightForScreenCapture(key: key)
    }
}

