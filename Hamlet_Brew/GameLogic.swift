//
//  MainScreen.swift
//  Hamlet_Brew
//
//  Created by Kelvin Kao on 8/1/19.
//  Copyright © 2019 Kelvin Kao. All rights reserved.
//

import Foundation
import UIKit

class GameLogic {
    enum GameState {
        case logo                   // splash screen
        case instructions           // what buttons to press to change set pieces
        case taleAboutTheRat        // in which hamlet tells gertrude about the rat
        case choices                // in which player decides whom hamlet kills
        case stabbing               // stab animation
        case ratReveal              // revelation of character killed
        case conclusion             // end of story
    }
    
    enum KillChoice: String, CaseIterable {
        case polonius = "Polonius"
        case kenny = "Kenny"
        case splinter = "Splinter"
    }
    
    let logoDuration = 2.0
    let instructionsDuration = 3.0
    let taleAboutRatDuration = 10.0
    let stabHoldDuration = 2.0
    let ratRevealHoldDuration = 4.0
    
    var outdoorImageName: String = "back0"
    var wallImageName: String = "wall0"
    var currentState: GameState = .logo
    var killChoice: KillChoice = .polonius
    
    let brewScreen: BrewScreen
    
    init(device: BrewDevice) {
        self.brewScreen = device.brewScreen
        device.delegate = self
    }
    
    func startInInitialState() {
        showGameState(state: .logo)
    }
    
    func makeKillChoice(_ choice: KillChoice) {
        killChoice = choice
        transitionToState(.stabbing, delay: 0.0)
    }
    
    func forceBackToMenuForScreenCapture() {
        transitionToState(.choices, delay: 0.0)
    }
}

// MARK: helpers for state transitions

fileprivate extension GameLogic {
    func showGameState(state: GameState) {
        currentState = state
        
        brewScreen.clearScreen()
        
        switch state {
        case .logo:
            drawLogo()
            transitionToState(.instructions, delay: logoDuration)
        case .instructions:
            drawInstructions()
            transitionToState(.taleAboutTheRat, delay: instructionsDuration)
        case .taleAboutTheRat:
            drawTaleAboutTheRat()
            transitionToState(.choices, delay: taleAboutRatDuration)
        case .choices:
            drawChoices()
            brewScreen.menu?.delegate = self
            brewScreen.menu?.activate()
        case .stabbing:
            drawStabbing()
        case .ratReveal:
            drawRatReveal()
        case .conclusion:
            drawConclusion()
        }
    }
    
    func transitionToState(_ state: GameState, delay: TimeInterval) {
        DispatchQueue.main.asyncAfter(deadline: .now() + delay) {
            self.showGameState(state: state)
        }
    }
}

// MARK: helpers for drawing each state

fileprivate extension GameLogic {
    func drawLogo() {
        brewScreen.drawImage(literalName: "logo", atX: 1.0, y: 50.0)
    }
    
    func drawInstructions() {
        brewScreen.drawBoldLabel(text: "Instructions:", atX: 10.0, y: 30.0)
        brewScreen.drawLabel(text: "Press 1,2,3 to", atX: 10.0, y: 50.0)
        brewScreen.drawLabel(text: "change landscape", atX: 10.0, y: 65.0)
        brewScreen.drawLabel(text: "Press 4,5,6 to", atX: 10.0, y: 85.0)
        brewScreen.drawLabel(text: "change the wall", atX: 10.0, y: 100.0)
    }
    
    func drawTaleAboutTheRat() {
        drawSetPieces()
        drawMainCharacters()
        drawTextBox(text: "Queen Gertrude:  Hamlet, o, Hamlet.  Wherefore art thou staring at my curtain?\nHamlet:  For I've found a rat.  Die, rat, die!")
    }
    
    func drawChoices() {
        drawSetPieces()
        drawMainCharacters()
        drawMenu(title: "Who does Hamlet Kill?",
                 menuItems: KillChoice.allCases.map({$0.rawValue}))
    }
    
    func drawStabbing() {
        drawTextBox(text: "Hamlet stabs the rat")
        
        drawStabbingWithSword(literalName: "sword1")
        
        DispatchQueue.main.asyncAfter(deadline: .now() + 0.5) {
            self.drawStabbingWithSword(literalName: "sword2")
            
            DispatchQueue.main.asyncAfter(deadline: .now() + 0.5, execute: {
                self.drawStabbingWithSword(literalName: "sword3")
                
                self.transitionToState(.ratReveal, delay: self.stabHoldDuration)
            })
        }
    }
    
    func drawRatReveal() {
        let text: String
        switch killChoice {
        case .polonius:
            text = "Polonius: Oy, I am slain, bleh.\n*Polonius falls*"
        case .kenny:
            text = "Kenny: Mmmmmmmm.\n*Kenny dies*"
        case .splinter:
            text = "Splinter: Why? Why me?\n*Splinter falls*"
        }
        drawTextBox(text: text)
        
        let slainCharacter = killChoice.rawValue.lowercased()
        
        drawRatRevealWithSlainCharacter(literalName: "\(slainCharacter)1")
        
        DispatchQueue.main.asyncAfter(deadline: .now() + 0.2) {
            self.drawRatRevealWithSlainCharacter(literalName: "\(slainCharacter)2")
            
            DispatchQueue.main.asyncAfter(deadline: .now() + 0.3, execute: {
                self.drawRatRevealWithSlainCharacter(literalName: "\(slainCharacter)3")
                
                self.transitionToState(.conclusion, delay: self.ratRevealHoldDuration)
            })
        }
    }
    
    func drawConclusion() {
        switch killChoice {
        case .polonius:
            drawTextBox(text: "Queen Gertrude: *cries* Unbelievable!  Thou has wrecked my favorite curtain!  Consider thyself disowned!  ~THE END~")
            drawSetPieces()
            drawMainCharacters()
            brewScreen.drawImage(literalName: "teardrops", atX: 69.0, y: 38.0)
        case .kenny:
            drawTextBox(text: "Stan: Oh my god! They killed Kenny!\nKyle: You bastards!\n~THE END~")
            brewScreen.drawImage(literalName: "stankyle", atX: 0.0, y: 0.0)
        case .splinter:
            drawTextBox(text: "Splinter:  Turtles, you must avenge my death!\nDa Vinci:  Aww.. Do we have to?\n~THE END~")
            brewScreen.drawImage(literalName: "turtles", atX: 0.0, y: 0.0)
        }
    }
}

fileprivate extension GameLogic {
    func drawSetPieces() {
        brewScreen.drawImage(literalName: outdoorImageName, atX: 85.0, y: 25.0)
        brewScreen.drawImage(literalName: wallImageName, atX: 0.0, y: 0.0)
    }
    
    func drawMainCharacters() {
        brewScreen.drawImage(literalName: "hamlet", atX: 21.0, y: 42.0)
        brewScreen.drawImage(literalName: "gertrude", atX: 53.0, y: 27.0)
    }
    
    func drawTextBox(text: String) {
        brewScreen.drawTextBox(text: text, atX: 3.0, y: 85.0)
    }
    
    func drawMenu(title: String, menuItems: [String]) {
        brewScreen.drawMenu(title: title, menuItems: menuItems, atY: 85.0)
    }
    
    func drawStabbingWithSword(literalName: String) {
        drawSetPieces()
        drawMainCharacters()
        brewScreen.drawImage(literalName: literalName, atX: 5.0, y: 52.0)
    }
    
    func drawRatRevealWithSlainCharacter(literalName: String) {
        drawSetPieces()
        drawMainCharacters()
        brewScreen.drawImage(literalName: literalName, atX: 0.0, y: 37.0)
    }
}

// MARK: handle menu selection

extension GameLogic: BrewMenuDelegate {
    func brewMenu(_: BrewMenu, didSelectMenuItem menuItem: String) {
        if let choice = KillChoice(rawValue: menuItem) {
            killChoice = choice
        }
        transitionToState(.stabbing, delay: 0.0)
        
        brewScreen.menu?.delegate = nil
        brewScreen.menu?.deactivate()
    }
}

// MARK: handle key press

extension GameLogic: BrewDeviceDelegate {
    func brewDevice(_: BrewDevice, didPressKey key: BrewKeyPad.Key) {
        switch key {
        case .num1:
            outdoorImageName = "back1"
        case .num2:
            outdoorImageName = "back2"
        case .num3:
            outdoorImageName = "back3"
        case .num4:
            wallImageName = "wall1"
        case .num5:
            wallImageName = "wall2"
        case .num6:
            wallImageName = "wall3"
        default:
            break
        }
        
        switch currentState {
        case .taleAboutTheRat, .choices:
            drawSetPieces()
            drawMainCharacters()
        default:
            break
        }
    }
}
