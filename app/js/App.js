import React, { Component } from 'react';
import AppHeader from './AppHeader';
import AppContent from './AppContent';
import AppFooter from './AppFooter';

class App extends Component {
	constructor(props) {
		super(props);
		this.state = { 
            lang: 'ru'
        };
		this.changeLanguage = this.changeLanguage.bind(this);        
    }
    
    changeLanguage() {
        if( this.state.lang == 'en' ) {
            this.setState( {lang:'ru'} );
        } else {
            this.setState( {lang:'en'} );
        }
    }

    render(){
      return(
         <div>
            <AppHeader lang={this.state.lang} changeLanguage={this.changeLanguage} />
			<AppContent lang={this.state.lang} />
			<AppFooter/>
         </div>
      );
   }
}

export default App;