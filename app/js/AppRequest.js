import React, { Component } from 'react';
import styles from './../css/apprequest.css'; 

class AppRequest extends React.Component {
	
	constructor(props) {
		super(props);
		this.state = { 
			loggedInAs: { userName:null, sessId:null },
			showRequestMessage: false,
			requestMessage: '',
			requestMessageClass: 'normal',
			response: 'No requests made yet...'
		};
		this.makeRequest = this.makeRequest.bind(this);
	}

	makeRequest() {
    	this.setState(state => ({
      		showRequestMessage: true,
			requestMessage: 'Making request...',
			requestMessageClass: 'normal'
    	}));

		const requestOptions = {
        	method: 'POST',
        	headers: { 'Content-Type': 'application/json' },
        	body: document.getElementById('request').value
    	};
		fetch("/api", requestOptions)
			.then( response => response.json() )
			.then(  data => this.setState( { showRequestMessage:false, response: JSON.stringify(data) } ) )
			.catch( error => this.setState( { showRequestMessage:true, requestMessage:'Error making a request...', response: '' } ) ); 
	}

	render() {
		let loggedInAs = this.state.loggedInAs.userName === null ? '[ You are not logged in... ]' : 
			`<br/>User: ${this.state.loggedInAs.userName}<br/>Session Id: ${this.state.loggedInAs.userName}`;
		return (
			<div className={styles.container}>
				<div className={styles.loggedInAs}>
					<b>Logged in as</b>: { loggedInAs }
				</div>
				<div className={styles.requestDialogContainer}>
					<div className={styles.requestDialogButtonsContainer}>
						<br/>
						<button onClick={this.makeRequest}>Make Request</button>
					</div>
					<div className={styles.requestDialogRequestContainer}>	
						<b>Request</b><br/>
						<textarea rows="4" id='request'></textarea>
						{ this.state.showRequestMessage ? 
							(<div className={styles.requestDialogRequestMessage}>{this.state.requestMessage}</div>) : null }
					</div>
					<div className={styles.requestDialogResponseContainer}>
						<b>Response</b><br/>
						<textarea rows="4" id='response' value={this.state.response}></textarea>
					</div>
				</div>
			</div>
		);
	}
}

export default AppRequest;