import React, { Component } from 'react';
import AppAPI from './AppAPI';
import AppRequest from './AppRequest';
import styles from './../css/appcontent.css'; 
import Settings from './Settings';

class AppContent extends React.Component {
	constructor(props) {
		super(props);
		this.state = { 
			requestText: '',
            loggedInAs: { user:null, sessId:null },
		};

		this.updateRequestText = this.updateRequestText.bind(this);
		this.updateLoggedInAs = this.updateLoggedInAs.bind(this);
		this.requestTextareaChanged = this.requestTextareaChanged.bind(this);
	}

	requestTextareaChanged(e) {
		this.setState( { requestText: e.target.value } ); 
	}
	
	updateRequestText( requestJSON ) {
		if( this.state.loggedInAs.user !== null && this.state.loggedInAs.sessId !== null ) {
			if( 'user' in requestJSON ) {
				requestJSON.user = this.state.loggedInAs.user;	
			}
			if( 'sessId' in requestJSON ) {
				requestJSON.sessId = this.state.loggedInAs.sessId;
			}
		}
        let requestText = JSON.stringify(requestJSON);
    	this.setState( { requestText: requestText } );
	}

	updateLoggedInAs( user, sessId ) {
		if( user.length === 0 ) {
			user = null;
		}
		if( sessId.length === 0 ) {
			sessId = null;
		}
    	this.setState( state => ( { loggedInAs: { user: user, sessId: sessId } } ) );
	}

	render() {
		return (
			<div className={styles.container}>
                <AppRequest lang={this.props.lang} updateLoggedInAs={this.updateLoggedInAs} 
                    loggedInAs={this.state.loggedInAs} requestText={this.state.requestText} 
                    requestTextareaChanged={this.requestTextareaChanged} />
				<h4>{Settings.texts.apiList[this.props.lang]}</h4>
				<AppAPI lang={this.props.lang} updateRequestText={this.updateRequestText}/>
			</div>
		);
	}
}

export default AppContent;