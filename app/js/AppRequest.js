import React, { Component } from 'react';
import styles from './../css/apprequest.css'; 
import Settings from './Settings';

class AppRequest extends React.Component {
	
	constructor(props) {
		super(props);
		this.state = { 
			//loggedInAs: { user:null, sessId:null },
			showRequestMessage: false,
			requestMessage: '',
			requestMessageClass: 'normal',
			responseText: ''
		};
		this.makeRequest = this.makeRequest.bind(this);
	}

	makeRequest() {
    	this.setState(state => ({
      		showRequestMessage: true,
			requestMessage: 'Making request...',
			requestMessageClass: 'normal'
    	}));

		const requestHeaders = {
        	method: 'POST',
        	headers: { 'Content-Type': 'application/json', 'Origin': Settings.apiServerURL+'/' },
        	body: document.getElementById('request').value
    	};
		let url = '/api';
		if( Settings.apiServerURL.length > 0 ) {
			url = Settings.apiServerURL + url;
		} 
		fetch(url, requestHeaders)
			.then( response => response.json() )
			.then(  data => { 
				let id = null; // Trying to retrieve id from the request made					
				try {
					let o = JSON.parse( this.props.requestText );
					if( 'id' in o ) {
						id = o.id;
					}
				} catch(e) {;}									
				let separator = '--------\n';
				let title = (id !== null && id.length > 0) ? `${separator}[${id}]\n` : `${separator}`;				
				this.setState( { showRequestMessage:false, responseText: title + JSON.stringify(data) + '\n' + this.state.responseText } ); 
				if( 'user' in data && 'sess_id' in data ) { 	// Just logged in?
					this.props.updateLoggedInAs( data.user, data.sess_id );
				}
			} )
			.catch( e => { 
				this.setState( { showRequestMessage:true, requestMessage:'Error making a request...' } ) 
			} ); 
	}

	render() {
		let user = this.props.loggedInAs.user === null ? 'Not authorized' : `${this.props.loggedInAs.user}`
		let sessId = this.props.loggedInAs.sessId === null ? 'Not authorized' : `${this.props.loggedInAs.sessId}`;
		return (
			<div className={styles.container}>
				<div className={styles.loggedInAs}>
					<div><b>Log in info</b>:</div>
					<div>User:&nbsp;<span>{user}</span></div>
					<div>Session&nbsp;Id:&nbsp;<span>{sessId}</span></div>
				</div>
				<div className={styles.requestDialogContainer}>
					<div className={styles.requestDialogSendingControlsContainer}>
						<br/>
						<button onClick={this.makeRequest}>Make Request</button>
					</div>
					<div className={styles.requestDialogRequestContainer}>	
						<b>Request</b><br/>
						<textarea rows="5" id='request' value={this.props.requestText} onChange={this.props.requestTextareaChanged}></textarea>
						{ this.state.showRequestMessage ? 
							(<div className={styles.requestDialogRequestMessage}>{this.state.requestMessage}</div>) : null }
					</div>
					<div className={styles.requestDialogResponseContainer}>
						<b>Response</b><br/>
						<textarea rows="5" id='response' value={this.state.responseText} readOnly></textarea>
					</div>
				</div>
			</div>
		);
	}
}

export default AppRequest;