import React, { Component } from 'react';
import styles from './../css/appapi.css'; 

class AppAPI extends React.Component {
	
	constructor(props) {
		super(props);
		this.state = { 
			itemChosenInAPIList: { id:null, title:'Not chosen', description:null, sample_request:null },
		};

		this.listItemChosen = this.listItemChosen.bind(this);
	}

	listItemChosen(e, id, title, description, sample_request) {
		this.setState( { itemChosenInAPIList: { id:id, title:title, description:description, sample_request:sample_request } } );
		this.props.updateRequestText( sample_request );
	}

	render() {
		return (
			<div className={styles.APIContainer}>
				<div className={styles.APIList}>
					<APIList onListItemChosen={this.listItemChosen} />
				</div>
				<div className={styles.APIItem}>
					<div className={styles.APIItemId}>
						{this.state.itemChosenInAPIList.id}
					</div>
					<div className={styles.APIItemTitle}>
						{this.state.itemChosenInAPIList.title}
					</div>
					<div className={styles.APIItemDescription}>
						{this.state.itemChosenInAPIList.description}
					</div>
					<div className={styles.APIItemSampleRequest}>
						{this.state.itemChosenInAPIList.sample_request}
					</div>
				</div>
			</div>
		);
	}
}


class APIList extends React.Component {
	constructor(props) {
    	super(props);
		this.state = {
			error: false,
			errorMessage: '',
			isLoaded: false,
			data: {}
		};	
	}

	componentDidMount() {
		fetch("/api_list")
			.then( response => response.json() )
			.then( data => { 
				this.setState( { error: false, isLoaded: true, data: data } ); 
				if( data.list.length > 0 ) {
					let item = data.list[0];
					this.props.onListItemChosen(null, item.id, item.title, item.description, JSON.stringify(item.sample_request));
				}
			} )
			.catch( e => { this.setState( { error:true, errorMessage: 'Error loading the API list!', isLoaded:true } ); } ); 
	}

	render() {
    	if (this.state.error) {
			return <div>Error loading API calls list</div>;
		} 
		else if ( !this.state.isLoaded ) {
			return <div>Please wait while loading API calls list...</div>;
		} 
		else {
			return (
				<div>
					{ 
					this.state.data.list.map( item => (
						<div key={item.id} className={styles.APIListItem} onClick={ (e) => 
							this.props.onListItemChosen(e, item.id, item.title, item.description, JSON.stringify(item.sample_request)) }>
							<span className={styles.APIListItemId}>{item.id}</span> :: {item.title}
						</div> ) ) 	
					}
				</div>
      		);
    	}
  	}
}

export default AppAPI;