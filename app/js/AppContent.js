import React, { Component } from 'react';
import AppAPI from './AppAPI';
import AppRequest from './AppRequest';
import styles from './../css/appcontent.css'; 

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
	
	updateRequestText( requestText ) {
		if( this.state.loggedInAs.user !== null && this.state.loggedInAs.sessId !== null ) {
		    let o = null;
			try {
				let o = JSON.parse( requestText );
				if( 'user' in o ) {
					o.user = this.state.loggedInAs.user;	
				}
				if( 'sess_id' in o ) {
					o.sess_id = this.state.loggedInAs.sessId;
				}
			} 
			catch(e) { 
				o = null; 
			}
			if( o !== null ) {
				requestText = JSON.stringify(o);
			}
		}
    	this.setState( state => ( { requestText: requestText } ) );
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
				<AppRequest updateLoggedInAs={this.updateLoggedInAs} loggedInAs={this.state.loggedInAs} 
					requestText={this.state.requestText} requestTextareaChanged={this.requestTextareaChanged} />
				<h4>API List</h4>
				<AppAPI updateRequestText={this.updateRequestText}/>
			</div>
		);
	}
}

export default AppContent;